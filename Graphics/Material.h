#pragma once
#include <DirectXMath.h>
#include <string>
#include <memory>

#include "Texture.h"

using namespace DirectX;

struct TextureInfo
{
    std::string path;
    int texCoord = 0;
};

class Material
{
public:
    // ---- 基本色（PBR）----
    XMFLOAT4 baseColor = { 1,1,1,1 };
    TextureInfo baseColorTex;
    std::shared_ptr<Texture> baseColorTexture;

    // ---- Metallic / Roughness ----
    float metallic = 0.0f;
    float roughness = 1.0f;
    TextureInfo metallicRoughnessTex;

    // ---- Normal map ----
    TextureInfo normalTex;

    // ---- Occlusion ----
    TextureInfo occlusionTex;

    // ---- Emissive ----
    XMFLOAT3 emissiveColor = { 0,0,0 };
    TextureInfo emissiveTex;

    // ---- Phong 用（Assimp 互換）----
    XMFLOAT3 diffuseColor = { 1,1,1 };
    TextureInfo diffuseTex;

    XMFLOAT3 specularColor = { 1,1,1 };
    TextureInfo specularTex;

    float shininess = 32.0f;

    // ---- フラグ ----
    bool doubleSided = false;
    std::string alphaMode = "OPAQUE";
    float alphaCutoff = 0.5f;

    // ---- Renderer 用 ----
    bool usePBR = false;
    bool useTexture = false;
};
