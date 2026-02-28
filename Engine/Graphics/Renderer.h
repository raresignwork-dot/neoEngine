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
#include "Graphics/GraphicDevice.h"

class Renderer
{
public:

    Renderer(GraphicDevice* device);
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


private:

    void TraverseNode(const ModelBase& model,
        const Node* node,
        const DirectX::XMMATRIX& parentMatrix);

    void DrawMesh(const ModelBase& model,
        const Mesh& mesh,
        const DirectX::XMMATRIX& world);

    void TraverseNodeSkinned(const SkinnedModel& model,
        const Node* node,
        const DirectX::XMMATRIX& parentMatrix);

    void DrawMeshSkinned(const Mesh& mesh,
        const DirectX::XMMATRIX& world,
        const std::vector<DirectX::XMMATRIX>& finalMatrices);

private:

    // GPU
    GraphicDevice* device_ = nullptr;

    // RenderTargets
    ID3D11RenderTargetView* renderTargetView_ = nullptr;
    ID3D11DepthStencilView* depthStencilView_ = nullptr;

    // States
    ComPtr<ID3D11RasterizerState> rasterizerState_;
    ComPtr<ID3D11DepthStencilState> depthStencilState_;

    // Shader
    std::unique_ptr<Shader> shader3D_;
    const Camera* camera_ = nullptr;

    // Buffers
    ComPtr<ID3D11Buffer> perObjectCB_;
    ComPtr<ID3D11Buffer> perFrameCB_;
    ComPtr<ID3D11Buffer> boneCB_;
    ComPtr<ID3D11Buffer> materialCB_;

    ComPtr<ID3D11SamplerState> samplerState_;

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
        DirectX::XMFLOAT4 baseColor;
        DirectX::XMFLOAT3 diffuseColor;
        float shininess;

        DirectX::XMFLOAT3 specularColor;
        float metallic;

        float roughness;
        int usePBR;
        int useTexture;
        int pad2;
    };
};