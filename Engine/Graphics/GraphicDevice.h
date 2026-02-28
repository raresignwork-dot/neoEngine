#pragma once

#include <Windows.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <d2d1_1.h>
#include <dwrite.h>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")


using Microsoft::WRL::ComPtr;

class GraphicDevice
{
public:
    GraphicDevice();
    ~GraphicDevice();

    bool Init(HINSTANCE hInstance, int width, int height , HWND hWnd);
    bool InitDirect2D();



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

};

