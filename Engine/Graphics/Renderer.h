#pragma once

#include <memory>
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

#include "Scene/Camera.h"
#include "Graphics/Model.h"
#include "Graphics/Mesh.h"
#include "Logic/Transform.h"
#include "Shaders/Shader.h"

class Renderer
{
public:
    Renderer(ID3D11Device* device, ID3D11DeviceContext* context);
    ~Renderer() = default;

    static const int MAX_BONES = 128;

    struct BoneCB
    {
        DirectX::XMMATRIX bones[MAX_BONES];
    };

    void Initialize();
    void Begin(const float clearColor[4]);
    void End();
    void SetCamera(const Camera* camera);
    void DrawModel(const ModelBase& model, Transform& transform);

    void SetRenderTargets(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
    {
        renderTargetView_ = rtv;
        depthStencilView_ = dsv;
    }

private:
    void TraverseNode(const ModelBase& model, const Node* node, const DirectX::XMMATRIX& parentMatrix);
    void DrawMesh(const ModelBase& model, const Mesh& mesh, const DirectX::XMMATRIX& world);

    void TraverseNodeSkinned(const SkinnedModel& model, const Node* node, const DirectX::XMMATRIX& parentMatrix);
    void DrawMeshSkinned(const Mesh& mesh,
        const DirectX::XMMATRIX& world,
        const std::vector<DirectX::XMMATRIX>& finalMatrices);

private:
    // --- デバイスとコンテキスト ---
    ComPtr<ID3D11Device> device_;
    ComPtr<ID3D11DeviceContext> context_;

    // --- レンダーターゲット関連 ---
    ComPtr<ID3D11RenderTargetView> renderTargetView_;
    ComPtr<ID3D11DepthStencilView> depthStencilView_;

    // --- 描画ステート (D2D対策) ---
    ComPtr<ID3D11RasterizerState> rasterizerState_;
    ComPtr<ID3D11DepthStencilState> depthStencilState_;

    // --- シェーダー・カメラ ---
    std::unique_ptr<Shader> shader3D_;
    const Camera* camera_ = nullptr;

    // --- 定数バッファ ---
    ComPtr<ID3D11Buffer> perObjectCB_;
    ComPtr<ID3D11Buffer> perFrameCB_;
    ComPtr<ID3D11Buffer> boneCB_;
    ComPtr<ID3D11Buffer> materialCB_;
    ComPtr<ID3D11SamplerState> samplerState_;

    // --- 定数バッファ用構造体定義 ---
    struct PerObjectCB
    {
        DirectX::XMMATRIX world;
    };

    struct PerFrameCB
    {
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
        DirectX::XMFLOAT3 lightDir;
        float pad0;
        DirectX::XMFLOAT3 cameraPos;
        float pad1;
    };

    struct MaterialCB
    {
        DirectX::XMFLOAT4 baseColor;    // 16 bytes
        DirectX::XMFLOAT3 diffuseColor; // 12 bytes
        float shininess;               // 4 bytes
        DirectX::XMFLOAT3 specularColor;// 12 bytes
        float metallic;                // 4 bytes
        float roughness;               // 4 bytes
        int usePBR;                    // 4 bytes
        int useTexture;                // 4 bytes
        int pad2;                      // 4 bytes
    };
};