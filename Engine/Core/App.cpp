#include "App.h"
#include "Scene/OpenningScene.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using Microsoft::WRL::ComPtr;

App::App() {}
App::~App() {}

bool App::Init(HINSTANCE hInstance, int width, int height)
{
    // =========================
    // Window
    // =========================
    WNDCLASSEX wc{};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = L"DX11WindowClass";

    RegisterClassEx(&wc);

    hWnd = CreateWindowEx(
        0,
        L"DX11WindowClass",
        L"DirectX11 Engine",
        WS_OVERLAPPEDWINDOW,
        100, 100,
        width, height,
        nullptr, nullptr,
        hInstance,
        nullptr);

    if (!hWnd) return false;

    ShowWindow(hWnd, SW_SHOW);

    // =========================
    // Timer
    // =========================
    QueryPerformanceFrequency(&m_frequency);
    QueryPerformanceCounter(&m_prevTime);

    // =========================
    // GraphicDevice
    // =========================
    GrapDevice = std::make_unique<GraphicDevice>();
    GrapDevice->Init(hInstance, width, height , hWnd);
    GrapDevice->InitDirect2D();


    shaderManager = std::make_unique<ShaderManager>(GrapDevice->m_pDevice.Get());

    shaderManager->Load("default", L"base.hlsl", "VSMain", "PSMain");
    //shaderManager->Load("unlit", L"unlit.hlsl", "VSMain", "PSMain");



    // Camera Settings

    // ========================= // SceneContext の基本情報 // ========================= 

    m_sceneContext.grapDevice = GrapDevice.get();
    m_sceneContext.settings = &m_globalSettings; 
    m_sceneContext.screenWidth = width;
    m_sceneContext.screenHeight = height;

    // ========================= // Renderer を作成 // ========================= 
    m_sceneContext.renderer = 
        std::make_shared<Renderer>(GrapDevice.get());
    m_sceneContext.renderer->Initialize(); 
    // Provide render target and depth stencil to renderer
    //m_sceneContext.renderer->SetRenderTargets(GrapDevice->m_pRenderTargetView.Get(), GrapDevice->m_pDepthStencilView.Get());

    // ========================= // Camera 設定（メンバ m_mainCamera） // ========================= 
    m_mainCamera.transform.position = { 0.0f, 0.0f, -3.0f }; 
    m_mainCamera.transform.rotation = { 0.0f, 0.0f, 0.0f }; 
    m_mainCamera.transform.scale = { 1.0f, 1.0f, 1.0f }; 
    m_mainCamera.transform.Update(XMMatrixIdentity()); 
    m_mainCamera.fov = XM_PI / 4.0f; 
    m_mainCamera.aspect = static_cast<float>(width) / static_cast<float>(height); 
    m_mainCamera.nearZ = 0.1f; 
    m_mainCamera.farZ = 1000.0f; 
    // SceneContext にカメラを渡す 
    m_sceneContext.camera = &m_mainCamera; 
    // // Renderer にカメラをセット 
    m_sceneContext.renderer->SetCamera(m_sceneContext.camera); 
    // ========================= // SceneManager // ========================= 
    m_pSceneManager = std::make_unique<SceneManager>(); 
    m_pSceneManager->ChangeScene<OpenningScene>();

    return true;
}

void App::Update()
{
    m_sceneContext.deltaTime = CalculateDeltaTime();

    if (m_pSceneManager)
        m_pSceneManager->Update(m_sceneContext);
}

void App::Draw()
{
    // Use black clear to avoid white flash on startup
    float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    // Let renderer handle per-frame setup (including setting per-frame constant buffer)
    if (m_sceneContext.renderer)
        m_sceneContext.renderer->Begin(clearColor);

    // 3D
    if (m_pSceneManager)
        m_pSceneManager->Draw(m_sceneContext);

    if (m_sceneContext.renderer)
        m_sceneContext.renderer->End();

    // 2D
    GrapDevice->m_pD2DTarget->BeginDraw();

    if (m_pSceneManager)
        m_pSceneManager->Draw2D(m_sceneContext);

    GrapDevice->m_pD2DTarget->EndDraw();

    GrapDevice->m_pSwapChain->Present(1, 0);
}

float App::CalculateDeltaTime()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    float delta =
        static_cast<float>(currentTime.QuadPart - m_prevTime.QuadPart) /
        static_cast<float>(m_frequency.QuadPart);

    m_prevTime = currentTime;
    return delta;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        return 0;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}