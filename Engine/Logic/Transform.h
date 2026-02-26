#pragma once
#include <vector>
#include <memory>
#include <DirectXMath.h>

using namespace DirectX;

struct Transform
{
    XMFLOAT3 position{ 0,0,0 };
    XMFLOAT3 rotation{ 0,0,0 };
    XMFLOAT3 scale{ 1,1,1 };

    XMMATRIX world;
    Transform* parent = nullptr;
    std::vector<std::unique_ptr<Transform>> children;

    Transform()
        : world(XMMatrixIdentity())   // ★ これが重要
    {
    }

    XMMATRIX GetMatrix() const;
    void Update(const XMMATRIX& parentMatrix);
};
