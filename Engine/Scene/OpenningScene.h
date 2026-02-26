#pragma once

#include <memory>
#include <d3dcompiler.h>

#include "Scene/Scene.h"
#include "Shaders/Shader.h"

// ライブラリのリンク（プロパティ設定が面倒な場合はここに書くと楽です）
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

class OpenningScene : public Scene
{
public:
    bool Load(SceneContext& ctx) override;
    void Update(SceneContext& ctx) override;
    void Draw(SceneContext& ctx) override;
    void Draw2D(SceneContext& ctx) override;
    void Unload() override;

private:
    struct SimpleVertex
    {
        float Pos[3];
    };

    struct ConstantBuffer
    {
        float Offset[4];
    };

    std::unique_ptr<Shader> m_shader;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
    // debug states removed

    float m_timer = 0.0f;
};
