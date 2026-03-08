#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>

#include "Logic/Transform.h"

using namespace DirectX;


class Camera {
public:
    Transform transform;

    float fov = XM_PI / 4.0f;
    float aspect = 16.0f / 9.0f;
    float nearZ = 0.1f;
    float farZ = 1000.0f;

    XMMATRIX GetView() const
    {
        return XMMatrixInverse(nullptr, transform.GetMatrix());
    }

    XMMATRIX GetProjection() const
    {
        return XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ);
    }

    bool IsVisible(const BoundingBox& box) const
    {
        BoundingFrustum frustum;
        XMMATRIX viewProj = GetView() * GetProjection();

        BoundingFrustum::CreateFromMatrix(frustum, viewProj);

        return frustum.Intersects(box);
    }
};