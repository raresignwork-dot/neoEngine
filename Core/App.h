// App.h
#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include "Scene/SceneManager.h"
#include "Scene/Camera.h"
#include "Scene/Scene.h"
#include "Graphics/Renderer.h"
#include "Graphics/GraphicDevice.h"
#include "Shaders/ShaderManager.h"

using Microsoft::WRL::ComPtr;

class App {
public:
    App();
    ~App();

    bool Init(HINSTANCE hInstance, int width, int height);
    void Update();
    void Draw();

private:
    HWND hWnd = nullptr;

    // ★ カメラはクラスメンバとして 1 個だけ
    Camera m_mainCamera;


    std::unique_ptr<GraphicDevice> GrapDevice;
    std::unique_ptr<SceneManager> m_pSceneManager;
    std::unique_ptr<ShaderManager> shaderManager;


    GlobalSettings m_globalSettings;
    SceneContext m_sceneContext;

    LARGE_INTEGER m_frequency{};
    LARGE_INTEGER m_prevTime{};

    //bool InitDirect2D();
    float CalculateDeltaTime();
};


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
