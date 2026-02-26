#include "Shaders/Shader.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <filesystem>

#include "Graphics/Vertex.h"

#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

bool Shader::CompileShader(
    const std::wstring& filePath,
    const std::string& entryPoint,
    const std::string& target,
    ID3DBlob** blob)
{
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG)
    flags |= D3DCOMPILE_DEBUG;
#endif

    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompileFromFile(
        filePath.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        entryPoint.c_str(),
        target.c_str(),
        flags,
        0,
        blob,
        errorBlob.GetAddressOf()
    );

    if (FAILED(hr))
    {
        if (errorBlob)
        {
            // ここでブレークポイントを張って、message の中身を直接見てください
            std::string message = (char*)errorBlob->GetBufferPointer();
            OutputDebugStringA("--- Shader Compile Error ---\n");
            OutputDebugStringA(message.c_str());
            OutputDebugStringA("----------------------------\n");
        }
        else if (hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
        {
            OutputDebugStringA("Error: Shader file not found.\n");
        }
        return false;
    }




    return true;
}

bool Shader::Load(
    ID3D11Device* device,
    const std::wstring& filePath,
    const std::string& vsEntry,
    const std::string& psEntry)
{

    // Shader::Load の冒頭

        if (!std::filesystem::exists(filePath)) {
            OutputDebugStringW((L"File NOT FOUND: " + filePath + L"\n").c_str());
            return false;
        }
        // ...

    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> psBlob;

    if (!CompileShader(filePath, vsEntry, "vs_5_0", vsBlob.GetAddressOf()))
    {
        OutputDebugStringA("VS compile failed\n");
        return false;
    }

    if (!CompileShader(filePath, psEntry, "ps_5_0", psBlob.GetAddressOf()))
    {
        OutputDebugStringA("PS compile failed\n");
        return false;
    }

    D3DWriteBlobToFile(vsBlob.Get(), L"VSMain.cso", TRUE);
    D3DWriteBlobToFile(psBlob.Get(), L"PSMain.cso", TRUE);

    // CreateVertexShader の HRESULT チェック
    if (!vsBlob)
        return false;

    HRESULT hr = device->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        vertexShader_.GetAddressOf()
    );
    if (FAILED(hr))
    {
        OutputDebugStringA("CreateVertexShader failed\n");
        return false;
    }

    // CreatePixelShader の HRESULT チェック
    if (!psBlob)
        return false;

    hr = device->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr,
        pixelShader_.GetAddressOf()
    );
    if (FAILED(hr))
    {
        OutputDebugStringA("CreatePixelShader failed\n");
        return false;
    }



    // 例: Shader::CreateInputLayout のあたり

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION",    0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(Vertex, position),    D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, offsetof(Vertex, normal),      D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD0",    0, DXGI_FORMAT_R32G32_FLOAT,       0, offsetof(Vertex, uv),          D3D11_INPUT_PER_VERTEX_DATA, 0 },
        // 例
        { "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "BLENDWEIGHT",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };




    // CreateInputLayout の HRESULT チェック
    hr = device->CreateInputLayout(
        layout,
        _countof(layout),
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        inputLayout_.GetAddressOf()
    );
    if (FAILED(hr)) {
        OutputDebugStringA("CreateInputLayout failed\n");
        return false;
    }


    return true;
}

void Shader::Bind(ID3D11DeviceContext* context)
{
    OutputDebugStringA("Shader::Bind: binding shaders and input layout\n");
    if (!context) return;
    if (inputLayout_) context->IASetInputLayout(inputLayout_.Get());
    if (vertexShader_) context->VSSetShader(vertexShader_.Get(), nullptr, 0);
    if (pixelShader_) context->PSSetShader(pixelShader_.Get(), nullptr, 0);
}
