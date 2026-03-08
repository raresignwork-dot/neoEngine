#pragma once
#include <wrl/client.h>
#include <d3d11.h>

class Texture
{
public:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv_;

    ID3D11ShaderResourceView* GetSRV() const
    {
        return srv_.Get();
    }
};
