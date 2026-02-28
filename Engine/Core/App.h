// App.h
#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include "Scene/SceneManager.h"
#include "Scene/Camera.h"
#include "Scene/Scene.h"
#include "Graphics/Renderer.h"
#include "Scene/Scene.h"
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
    HWND m_hWnd = nullptr;

    // ★ カメラはクラスメンバとして 1 個だけ
    Camera m_mainCamera;

    ComPtr<ID3D11Device> m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pImmediateContext;
    ComPtr<IDXGISwapChain> m_pSwapChain;
    ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
    ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;
    ComPtr<ID3D11RasterizerState> m_pRasterizerState;

    ComPtr<ID2D1Factory1> m_pD2DFactory;
    ComPtr<ID2D1Device> m_pD2DDevice;
    ComPtr<ID2D1DeviceContext> m_pD2DTarget;
    ComPtr<ID2D1Bitmap1> m_pD2DTargetBitmap;
    ComPtr<IDWriteFactory> m_pDWriteFactory;


    std::unique_ptr<SceneManager> m_pSceneManager;
    std::unique_ptr<ShaderManager> shaderManager;


    GlobalSettings m_globalSettings;
    SceneContext m_sceneContext;

    LARGE_INTEGER m_frequency{};
    LARGE_INTEGER m_prevTime{};

    bool InitDirect2D();
    float CalculateDeltaTime();
};


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
