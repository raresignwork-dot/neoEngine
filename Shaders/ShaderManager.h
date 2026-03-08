#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include <d3d11.h>              // ★ ID3D11Device の定義
#include <wrl/client.h>         // ★ ComPtr を使う場合

#include "Shaders/Shader.h"     // ★ Shader クラス

class ShaderManager
{
public:
    ShaderManager(ID3D11Device* device)
        : device_(device)
    {
    }

    std::shared_ptr<Shader> Load(
        const std::string& name,
        const std::wstring& file,
        const std::string& vs,
        const std::string& ps)
    {
        auto shader = std::make_shared<Shader>();
        if (!shader->Load(device_, file, vs, ps))
            return nullptr;

        shaders_[name] = shader;
        return shader;
    }

    std::shared_ptr<Shader> Get(const std::string& name)
    {
        auto it = shaders_.find(name);
        if (it != shaders_.end())
            return it->second;

        return nullptr;
    }

private:
    ID3D11Device* device_ = nullptr; // ★ デバイス保持
    std::unordered_map<std::string, std::shared_ptr<Shader>> shaders_;
};
