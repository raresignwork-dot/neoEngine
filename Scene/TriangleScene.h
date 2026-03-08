#pragma once
#include "Scene/Scene.h"
#include <d3dcompiler.h>

// ライブラリのリンク（プロパティ設定が面倒な場合はここに書くと楽です）
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

class TriangleScene : public Scene {
public:
    // 全て override をつけて親クラスとの不一致を防ぎます
    bool Load(SceneContext& ctx) override;
    void Update(SceneContext& ctx) override;
    void Draw(SceneContext& ctx) override;
    void Draw2D(SceneContext& ctx) override;
    void Unload() override;

private:
    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pConstantBuffer = nullptr;
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;
    ID3D11InputLayout* m_pVertexLayout = nullptr;
    float m_timer = 0.0f;
};