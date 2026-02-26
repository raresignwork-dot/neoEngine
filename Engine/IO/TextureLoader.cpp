#include "IO/TextureLoader.h"
#include "WICTextureLoader.h"   // DirectXTK

#include <DirectXHelpers.h>
#include <SimpleMath.h>

using Microsoft::WRL::ComPtr;

static std::wstring ToWString(const std::string& s)
{
    return std::wstring(s.begin(), s.end());
}

TextureLoader::TextureLoader(ID3D11Device* device)
    : device_(device)
{
}

std::shared_ptr<Texture> TextureLoader::Load(const std::string& path)
{
    std::wstring wpath = ToWString(path);

    ComPtr<ID3D11ShaderResourceView> srv;
    HRESULT hr = DirectX::CreateWICTextureFromFile(
        device_,
        wpath.c_str(),
        nullptr,
        srv.GetAddressOf()
    );

    if (FAILED(hr))
    {
        OutputDebugStringA(("Texture load failed: " + path + "\n").c_str());
        return nullptr;
    }

    auto tex = std::make_shared<Texture>();
    tex->srv_ = srv;
    return tex;
}
