#include "GraphicDevice.h"

//#include <d3d11.h>

GraphicDevice::GraphicDevice()
{
}

GraphicDevice::~GraphicDevice()
{
}

bool GraphicDevice::Init(HINSTANCE hInstance, int width, int height , HWND m_hWnd)
{

    // =========================
// D3D11 Device + SwapChain
// =========================
    UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // D2D共有必須
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_hWnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        creationFlags,
        nullptr, 0,
        D3D11_SDK_VERSION,
        &sd,
        m_pSwapChain.GetAddressOf(),
        m_pDevice.GetAddressOf(),
        nullptr,
        m_pImmediateContext.GetAddressOf());

    if (FAILED(hr)) return false;

    // =========================
    // RenderTargetView
    // =========================
    ComPtr<ID3D11Texture2D> backBuffer;

    hr = m_pSwapChain->GetBuffer(
        0,
        IID_PPV_ARGS(backBuffer.GetAddressOf()));
    if (FAILED(hr)) return false;

    hr = m_pDevice->CreateRenderTargetView(
        backBuffer.Get(),
        nullptr,
        m_pRenderTargetView.GetAddressOf());
    if (FAILED(hr)) return false;

    m_pSwapChain->Present(1, 0);

    // Depth stencil buffer (optional but commonly used)
    D3D11_TEXTURE2D_DESC depthDesc{};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ComPtr<ID3D11Texture2D> depthStencil;
    hr = m_pDevice->CreateTexture2D(&depthDesc, nullptr, depthStencil.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = m_pDevice->CreateDepthStencilView(depthStencil.Get(), nullptr, m_pDepthStencilView.GetAddressOf());
    if (FAILED(hr)) return false;

    ID3D11RenderTargetView* rtv = m_pRenderTargetView.Get();
    ID3D11DepthStencilView* dsv = m_pDepthStencilView.Get();
    m_pImmediateContext->OMSetRenderTargets(1, &rtv, dsv);

    D3D11_VIEWPORT vp{};
    vp.Width = static_cast<float>(width);
    vp.Height = static_cast<float>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    m_pImmediateContext->RSSetViewports(1, &vp);

    // Create default rasterizer state with back-face culling
    D3D11_RASTERIZER_DESC rsDesc{};
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.FrontCounterClockwise = FALSE;
    rsDesc.DepthClipEnable = TRUE;
    hr = m_pDevice->CreateRasterizerState(&rsDesc, m_pRasterizerState.GetAddressOf());
    if (FAILED(hr)) return false;
    m_pImmediateContext->RSSetState(m_pRasterizerState.Get());

    return true;

}

bool GraphicDevice::InitDirect2D()
{
    HRESULT hr;

    // Factory
    hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        m_pD2DFactory.GetAddressOf());
    if (FAILED(hr)) return false;

    // DXGI Device
    ComPtr<IDXGIDevice> dxgiDevice;
    hr = m_pDevice.As(&dxgiDevice);
    if (FAILED(hr)) return false;

    // D2D Device
    hr = m_pD2DFactory->CreateDevice(
        dxgiDevice.Get(),
        m_pD2DDevice.GetAddressOf());
    if (FAILED(hr)) return false;

    // D2D Context
    hr = m_pD2DDevice->CreateDeviceContext(
        D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
        m_pD2DTarget.GetAddressOf());
    if (FAILED(hr)) return false;

    // BackBuffer
    ComPtr<IDXGISurface> dxgiBackBuffer;
    hr = m_pSwapChain->GetBuffer(
        0,
        IID_PPV_ARGS(dxgiBackBuffer.GetAddressOf()));
    if (FAILED(hr)) return false;

    // Bitmap
    D2D1_BITMAP_PROPERTIES1 props =
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(
                DXGI_FORMAT_B8G8R8A8_UNORM, // ★統一
                D2D1_ALPHA_MODE_PREMULTIPLIED));

    hr = m_pD2DTarget->CreateBitmapFromDxgiSurface(
        dxgiBackBuffer.Get(),
        &props,
        m_pD2DTargetBitmap.GetAddressOf());
    if (FAILED(hr)) return false;

    m_pD2DTarget->SetTarget(m_pD2DTargetBitmap.Get());

    // DirectWrite
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(
            m_pDWriteFactory.GetAddressOf()));
    if (FAILED(hr)) return false;

    return true;
}

