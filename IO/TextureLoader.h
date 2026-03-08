#pragma once
#include <memory>
#include <string>
#include <wrl/client.h>
#include <d3d11.h>

#include "Graphics/Texture.h"

class TextureLoader
{
public:
    TextureLoader(ID3D11Device* device);
    std::shared_ptr<Texture> Load(const std::string& path);

private:
    ID3D11Device* device_;
};
