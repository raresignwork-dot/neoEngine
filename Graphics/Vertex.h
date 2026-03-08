
// Graphics/Vertex.h
#pragma once
#include <DirectXMath.h>
#include <cstdint> // uint32_t 用

using namespace DirectX;

struct Vertex
{
    XMFLOAT3 position;
    XMFLOAT3 normal;
    XMFLOAT2 uv;

    uint32_t boneIndices[4];
    float boneWeights[4];

    // コンストラクタで初期化するようにすれば安全です
    Vertex() {
        position = XMFLOAT3(0, 0, 0);
        normal = XMFLOAT3(0, 0, 0);
        uv = XMFLOAT2(0, 0);
        for (int i = 0; i < 4; ++i) {
            boneIndices[i] = 0;
            boneWeights[i] = 0.0f;
        }
    }
};