#include "OpenningScene.h"
#include "IO/GltfLoader.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <iostream> // 必要に応じて
#include <cstdlib> // EXIT_FAILURE
#include <cerrno>  // errno

#include <cassert>
#include <d2d1.h>
#include <dwrite.h>
#include <sstream>



using Microsoft::WRL::ComPtr;

bool OpenningScene::Load(SceneContext& ctx)
{
    if (ctx.renderer) ctx.renderer->SetCamera(ctx.camera);


    // Engine/Scene/OpenningScene.cpp のカメラ設定あたり

    auto* cam = ctx.camera;

    cam->transform.position = { 0.0f, 2.0f, -20.0f }; // 12mモデルを全体表示できる距離
    cam->transform.rotation = { 0.0f, 0.0f, 0.0f };  // 前向き
    cam->transform.Update(XMMatrixIdentity());



    cam->fov = XM_PI / 4.0f;
    cam->aspect = static_cast<float>(ctx.screenWidth) / static_cast<float>(ctx.screenHeight);
    cam->nearZ = 0.01f;
    cam->farZ = 2000.0f;



    auto obj = std::make_unique<GameObject>();

    // モデル読み込み
    GltfLoader loader(ctx.device.Get());
    auto model = loader.Load("Assets/Model/Player/TestPlayer.gltf");

    if (!model)
    {
        printf("Model load failed\n");
        return false;
    }

    // デバッグ出力
    model->DebugPrint();

    // GameObject にセット
    obj->model = model;

    // ★ オブジェクトの位置を設定
    obj->transform.position = { 0, 0, 0 };
    obj->transform.rotation = { 0.0f, 0.0f, 0.0f };
    obj->transform.scale = { 1.0f , 1.0f , 1.0f };
    obj->transform.Update(XMMatrixIdentity());


    objects.push_back(std::move(obj));

    return true;
}




void OpenningScene::Update(SceneContext&)
{
    m_timer += 0.02f;
}

void OpenningScene::Draw(SceneContext& ctx)
{
    for (auto& obj : objects)
    {
        ctx.renderer->DrawModel(*obj->GetModel(), obj->transform);
    }
}


void OpenningScene::Draw2D(SceneContext& ctx)
{
    ComPtr<ID2D1SolidColorBrush> brush;
    ctx.d2dContext->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::Yellow),
        brush.GetAddressOf());

    ctx.d2dContext->FillRectangle(
        D2D1::RectF(10, 10, 110, 110),
        brush.Get());

    ComPtr<IDWriteTextFormat> textFormat;
    ctx.dwriteFactory->CreateTextFormat(
        L"Consolas",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        32.0f,
        L"ja-jp",
        textFormat.GetAddressOf());

    ComPtr<ID2D1SolidColorBrush> whiteBrush;
    ctx.d2dContext->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        whiteBrush.GetAddressOf());

    const wchar_t* msg = L"Hello Direct2D!";

    ctx.d2dContext->DrawText(
        msg,
        (UINT32)wcslen(msg),
        textFormat.Get(),
        D2D1::RectF(15, 15, 500, 100),
        whiteBrush.Get());
}

void OpenningScene::Unload()
{
    m_shader.reset();
    m_vertexBuffer.Reset();
    m_constantBuffer.Reset();
}
