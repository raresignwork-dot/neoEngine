#pragma once

#include <vector>
#include <array>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>




#include "Graphics/Vertex.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

class Mesh
{
public:
    Mesh() = default;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    int materialIndex = 0;


    // GPU バッファ
    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;

    // ---- ボーンウェイト追加 ----
    void AddBoneWeight(uint32_t vertexID, int boneID, float weight)
    {
        auto& v = vertices[vertexID];

        for (int i = 0; i < 4; i++)
        {
            if (v.boneWeights[i] == 0.0f)
            {
                v.boneIndices[i] = boneID;
                v.boneWeights[i] = weight;
                return;
            }
        }

        // 4つ埋まっている場合は最小のウェイトを置き換える
        int minIndex = 0;
        for (int i = 1; i < 4; i++)
        {
            if (v.boneWeights[i] < v.boneWeights[minIndex])
                minIndex = i;
        }

        v.boneIndices[minIndex] = boneID;
        v.boneWeights[minIndex] = weight;
    }

    // ---- GPU バッファ生成 ----
    void CreateBuffers(ID3D11Device* device);

    // ---- Renderer 用 ----
    ID3D11Buffer* GetVertexBuffer() const { return vertexBuffer.Get(); }
    ID3D11Buffer* GetIndexBuffer() const { return indexBuffer.Get(); }
    UINT GetStride() const { return sizeof(Vertex); }
    UINT GetIndexCount() const { return (UINT)indices.size(); }
};
