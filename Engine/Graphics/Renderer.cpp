#include "Renderer.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <algorithm>

#include "../Scene/Camera.h"
#include "../Scene/Node.h"
#include "../Graphics/Model.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "../Shaders/Shader.h"
#include "../Animation/Animation.h"


#include <wrl/client.h>
#include "Texture.h"

using namespace DirectX;
using Microsoft::WRL::ComPtr;

Renderer::Renderer(GraphicDevice* device)
    : device_(device)
{
}


void Renderer::Initialize()
{
    shader3D_ = std::make_unique<Shader>();
    // パスは環境に合わせて調整してください
    shader3D_->Load(device_->m_pDevice.Get(), L"Assets/Shaders/base.hlsl", "VSMain", "PSMain");

    // --- 定数バッファ作成 (PerObject) ---
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.ByteWidth = sizeof(PerObjectCB);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        device_->m_pDevice->CreateBuffer(&desc, nullptr, perObjectCB_.GetAddressOf());
    }

    // --- 定数バッファ作成 (PerFrame) ---
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.ByteWidth = sizeof(PerFrameCB);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        device_->m_pDevice->CreateBuffer(&desc, nullptr, perFrameCB_.GetAddressOf());
    }

    // --- 定数バッファ作成 (Bone) ---
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.ByteWidth = sizeof(BoneCB);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        device_->m_pDevice->CreateBuffer(&desc, nullptr, boneCB_.GetAddressOf());
    }

    // --- 定数バッファ作成 (Material) ---
    {
        D3D11_BUFFER_DESC desc;
        ZeroMemory(&desc, sizeof(desc));
        desc.ByteWidth = sizeof(MaterialCB);
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        device_->m_pDevice->CreateBuffer(&desc, nullptr, materialCB_.GetAddressOf());
    }

    // --- ラスタライザステート (D2D対策) ---
    {
        D3D11_RASTERIZER_DESC rsDesc;
        ZeroMemory(&rsDesc, sizeof(rsDesc));
        rsDesc.FillMode = D3D11_FILL_SOLID;
        rsDesc.CullMode = D3D11_CULL_BACK;
        rsDesc.DepthClipEnable = TRUE;
        device_->m_pDevice->CreateRasterizerState(&rsDesc, rasterizerState_.GetAddressOf());
    }

    // --- 深度ステンシルステート (D2D対策) ---
    {
        D3D11_DEPTH_STENCIL_DESC dsDesc;
        ZeroMemory(&dsDesc, sizeof(dsDesc));
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
        device_->m_pDevice->CreateDepthStencilState(&dsDesc, depthStencilState_.GetAddressOf());
    }

    // --- サンプラ ---
    {
        D3D11_SAMPLER_DESC samp;
        ZeroMemory(&samp, sizeof(samp));
        samp.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samp.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samp.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samp.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samp.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        samp.MaxLOD = D3D11_FLOAT32_MAX;
        device_->m_pDevice->CreateSamplerState(&samp, samplerState_.GetAddressOf());
    }
}

void Renderer::Begin(const float clearColor[4])
{
    ID3D11RenderTargetView* rt = device_->m_pRenderTargetView.Get();
    ID3D11DepthStencilView* ds = device_->m_pDepthStencilView.Get();
    device_->m_pImmediateContext->OMSetRenderTargets(1, &rt, ds);

    if (rt) device_->m_pImmediateContext->ClearRenderTargetView(rt, clearColor);
    if (ds) device_->m_pImmediateContext->ClearDepthStencilView(ds, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    device_->m_pImmediateContext->RSSetState(rasterizerState_.Get());
    device_->m_pImmediateContext->OMSetDepthStencilState(depthStencilState_.Get(), 0);
    device_->m_pImmediateContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

    D3D11_VIEWPORT vp;
    ZeroMemory(&vp, sizeof(vp));
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width = 1280.0f;
    vp.Height = 720.0f;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    device_->m_pImmediateContext->RSSetViewports(1, &vp);

    device_->m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    if (camera_)
    {
        PerFrameCB cb;
        ZeroMemory(&cb, sizeof(cb));
        cb.view = XMMatrixTranspose(camera_->GetView());
        cb.projection = XMMatrixTranspose(camera_->GetProjection());
        cb.lightDir = XMFLOAT3(0.0f, -1.0f, 0.0f);
        cb.cameraPos = camera_->transform.position;
        cb.pad0 = 0.0f;
        cb.pad1 = 0.0f;

        device_->m_pImmediateContext->UpdateSubresource(perFrameCB_.Get(), 0, nullptr, &cb, 0, 0);

        ID3D11Buffer* p = perFrameCB_.Get();
        device_->m_pImmediateContext->VSSetConstantBuffers(1, 1, &p);
        device_->m_pImmediateContext->PSSetConstantBuffers(1, 1, &p);
    }

    ID3D11SamplerState* samp = samplerState_.Get();
    device_->m_pImmediateContext->PSSetSamplers(0, 1, &samp);

    if (shader3D_)
        shader3D_->Bind(device_->m_pImmediateContext.Get());
}
void Renderer::SetCamera(const Camera* camera)
{
    camera_ = camera;
}



void Renderer::End()
{
    // 今は特に何もしない
}

void Renderer::DrawModel(const ModelBase& model, Transform& transform)
{
    if (!model.rootNode) return;

    XMMATRIX world = transform.GetMatrix();

    if (auto skinned = dynamic_cast<const SkinnedModel*>(&model))
    {
        TraverseNodeSkinned(*skinned, model.rootNode.get(), world);
    }
    else
    {
        TraverseNode(model, model.rootNode.get(), world);
    }
}


void Renderer::TraverseNode(const ModelBase& model, const Node* node, const XMMATRIX& parentMatrix)
{
    if (!node) return;

    XMMATRIX local = node->localTransform;
    XMMATRIX world = local * parentMatrix;

    // このノードに紐づくメッシュを描画
    for (int idx : node->meshIndices)
    {
        if (idx < 0 || idx >= static_cast<int>(model.meshes.size())) continue;
        const Mesh& mesh = model.meshes[idx];
        DrawMesh(model, mesh, world);
    }

    // 子ノードへ
    for (auto& child : node->children)
    {
        TraverseNode(model, child.get(), world);
    }
}

void Renderer::DrawMesh(const ModelBase& model, const Mesh& mesh, const XMMATRIX& world)
{
    // ---- Material ----
// DrawMesh 内
    MaterialCB mat{};
    if (mesh.materialIndex >= 0 && mesh.materialIndex < (int)model.materials.size())
    {
        const Material& m = model.materials[mesh.materialIndex];
        mat.baseColor = m.baseColor;
        mat.diffuseColor = m.diffuseColor;
        mat.specularColor = m.specularColor;
        mat.shininess = m.shininess;
        mat.metallic = m.metallic;
        mat.roughness = m.roughness;
        mat.usePBR = m.usePBR ? 1 : 0;

        // ここでテクスチャがあるか判定
        mat.useTexture = (m.baseColorTexture != nullptr) ? 1 : 0;
    }

    device_->m_pImmediateContext->UpdateSubresource(materialCB_.Get(), 0, nullptr, &mat, 0, 0);
    ID3D11Buffer* mcb = materialCB_.Get();
    device_->m_pImmediateContext->PSSetConstantBuffers(3, 1, &mcb);

    // テクスチャ
    if (mat.useTexture)
    {
        const Material& m = model.materials[mesh.materialIndex];
        ID3D11ShaderResourceView* srv = m.baseColorTexture->GetSRV();
        device_->m_pImmediateContext->PSSetShaderResources(0, 1, &srv);
    }

    // PerObject
    PerObjectCB obj{};
    obj.world = XMMatrixTranspose(world);
    device_->m_pImmediateContext->UpdateSubresource(perObjectCB_.Get(), 0, nullptr, &obj, 0, 0);
    ID3D11Buffer* pcb = perObjectCB_.Get();
    device_->m_pImmediateContext->VSSetConstantBuffers(0, 1, &pcb);

    UINT stride = mesh.GetStride();
    UINT offset = 0;

    ID3D11Buffer* vb = mesh.GetVertexBuffer();
    ID3D11Buffer* ib = mesh.GetIndexBuffer();
    if (!vb || !ib)
    {
        OutputDebugStringA("Renderer::DrawMesh: missing vertex or index buffer, skipping\n");
        return;
    }

    device_->m_pImmediateContext->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    device_->m_pImmediateContext->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
    device_->m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    device_->m_pImmediateContext->DrawIndexed(mesh.GetIndexCount(), 0, 0);
}

void Renderer::TraverseNodeSkinned(const SkinnedModel& model, const Node* node, const XMMATRIX& parentMatrix)
{
    if (!node) return;

    XMMATRIX local = node->localTransform;
    XMMATRIX world = local * parentMatrix;

    for (int idx : node->meshIndices)
    {
        if (idx < 0 || idx >= static_cast<int>(model.meshes.size())) continue;
        const Mesh& mesh = model.meshes[idx];
        DrawMeshSkinned(mesh, world, model.finalMatrices);
    }

    for (auto& child : node->children)
    {
        TraverseNodeSkinned(model, child.get(), world);
    }
}

void Renderer::DrawMeshSkinned(
    const Mesh& mesh,
    const XMMATRIX& world,
    const std::vector<XMMATRIX>& finalMatrices)
{
    BoneCB boneData{};
    size_t count = std::min<size_t>(finalMatrices.size(), MAX_BONES);
    for (size_t i = 0; i < count; ++i)
        boneData.bones[i] = XMMatrixTranspose(finalMatrices[i]);

    device_->m_pImmediateContext->UpdateSubresource(boneCB_.Get(), 0, nullptr, &boneData, 0, 0);
    ID3D11Buffer* boneCB = boneCB_.Get();
    device_->m_pImmediateContext->VSSetConstantBuffers(2, 1, &boneCB);

    // PerObject
    PerObjectCB obj{};
    obj.world = XMMatrixTranspose(world);
    device_->m_pImmediateContext->UpdateSubresource(perObjectCB_.Get(), 0, nullptr, &obj, 0, 0);
    ID3D11Buffer* pcb = perObjectCB_.Get();
    device_->m_pImmediateContext->VSSetConstantBuffers(0, 1, &pcb);

    UINT stride = mesh.GetStride();
    UINT offset = 0;

    ID3D11Buffer* vb = mesh.GetVertexBuffer();
    ID3D11Buffer* ib = mesh.GetIndexBuffer();
    if (!vb || !ib)
    {
        OutputDebugStringA("Renderer::DrawMeshSkinned: missing vertex or index buffer, skipping\n");
        return;
    }

    device_->m_pImmediateContext->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    device_->m_pImmediateContext->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
    device_->m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    device_->m_pImmediateContext->DrawIndexed(mesh.GetIndexCount(), 0, 0);
}


