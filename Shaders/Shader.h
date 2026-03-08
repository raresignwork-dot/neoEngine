#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include <string>

class Shader
{
public:
    Shader() = default;
    ~Shader() = default;

    bool Load(
        ID3D11Device* device,
        const std::wstring& filePath,
        const std::string& vsEntry,
        const std::string& psEntry
    );

    void Bind(ID3D11DeviceContext* context);

private:
    bool CompileShader(
        const std::wstring& filePath,
        const std::string& entryPoint,
        const std::string& target,
        ID3DBlob** blob
    );

private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader>  pixelShader_;
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>  inputLayout_;
};
