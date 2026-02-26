#pragma once

#include <vector>
#include <memory>

#include <wrl/client.h>
using Microsoft::WRL::ComPtr;


#include <d3d11.h>
#include <d2d1_1.h>
#include <dwrite.h>

#include "Graphics/Model.h"
#include "Graphics/Renderer.h"
#include "Logic/GameObject.h"
#include "Scene/Camera.h"


// 1. まず、中身を定義する
struct GlobalSettings {
    float masterVolume = 0.5f;
    bool isFullScreen = false;
    int currentScore = 0;
};

// 2. 次に、それを使う構造体を定義する
struct SceneContext {
    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;

    ComPtr<ID2D1DeviceContext> d2dContext;
    ComPtr<IDWriteFactory> dwriteFactory;

    GlobalSettings* settings = nullptr;
    std::shared_ptr<Renderer> renderer = nullptr;

    Camera* camera = nullptr;   // ★追加
    int screenWidth{ 0 };
    int screenHeight{ 0 };

    float deltaTime = 0;
};


// 3. 最後にクラス
class Scene {
public:
    virtual ~Scene() {}
    virtual bool Load(SceneContext& ctx) = 0;
    virtual void Update(SceneContext& ctx) = 0;
    virtual void Draw(SceneContext& ctx) = 0;     // 3D用
    virtual void Draw2D(SceneContext& ctx) = 0;   // ★2D用（追加）
    virtual void Unload() = 0;

    std::vector<std::unique_ptr<GameObject>> objects;

    GameObject* CreateObject(std::shared_ptr<ModelBase> model)
    {
        auto obj = std::make_unique<GameObject>();
        obj->model = model;

        objects.push_back(std::move(obj));
        return objects.back().get();
    }
};