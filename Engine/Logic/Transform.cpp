#include "Logic/Transform.h"

using namespace DirectX;

#include <DirectXMath.h>
using namespace DirectX;

XMMATRIX Transform::GetMatrix() const
{
    XMMATRIX t = XMMatrixTranslation(position.x, position.y, position.z);

    XMMATRIX r = XMMatrixRotationRollPitchYaw(
        rotation.x,
        rotation.y,
        rotation.z
    );

    XMMATRIX s = XMMatrixScaling(scale.x, scale.y, scale.z);

    // Scale → Rotate → Translate
    return s * r * t;
}

void Transform::Update(const XMMATRIX& parentMatrix)
{
    // 自分のローカル行列
    XMMATRIX local = GetMatrix();

    // ワールド行列
    world = local * parentMatrix;

    // 子へ伝播
    for (auto& child : children)
    {
        child->Update(world);
    }
}