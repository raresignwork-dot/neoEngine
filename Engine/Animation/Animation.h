#pragma once

#include <vector>
#include <string>
using namespace DirectX;


// ボーンの情報

struct BoneInfo
{
    std::string name;
    int id = -1;
    XMMATRIX offsetMatrix = XMMatrixIdentity();
};


// キーフレームの情報

struct KeyFrame
{
    float timeStamp;

    DirectX::XMFLOAT3 position = { 0,0,0 };
    DirectX::XMFLOAT4 rotation = { 0,0,0,0}; // quaternion
    DirectX::XMFLOAT3 scale = { 1,1,1 };
};

// keyframe data

struct KeyPosition
{
    DirectX::XMFLOAT3 position = { 0,0,0 };
    float timeStamp = 0.0f;
};


struct KeyRotation
{
    XMFLOAT4 rotation = { 0,0,0,0 };
    float timeStamp = 0.0f;
};

struct KeyScale
{
    XMFLOAT3 scale = { 0,0,0 };
    float timeStamp = 0.0f;
};

// animation data for a single node

struct NodeAnimation
{
    std::string nodeName;

    std::vector<KeyPosition> positions;
    std::vector<KeyRotation> rotations;
    std::vector<KeyScale> scales;
};


// animation data for the entire model

struct Animation
{
    float duration = 0.0f;
    float ticksPerSecond = 0.0f;

    std::vector<NodeAnimation> channels;
};


// Animation.h に追加する

inline XMMATRIX InterpolatePosition(const NodeAnimation& channel, float time)
{
    if (channel.positions.size() == 1)
    {
        return XMMatrixTranslation(
            channel.positions[0].position.x,
            channel.positions[0].position.y,
            channel.positions[0].position.z
        );
    }

    int index = 0;
    for (int i = 0; i < channel.positions.size() - 1; i++)
    {
        if (time < channel.positions[i + 1].timeStamp)
        {
            index = i;
            break;
        }
    }

    const auto& p0 = channel.positions[index];
    const auto& p1 = channel.positions[index + 1];

    float t = (time - p0.timeStamp) / (p1.timeStamp - p0.timeStamp);

    XMVECTOR v0 = XMLoadFloat3(&p0.position);
    XMVECTOR v1 = XMLoadFloat3(&p1.position);

    XMVECTOR pos = XMVectorLerp(v0, v1, t);

    XMFLOAT3 result;
    XMStoreFloat3(&result, pos);

    return XMMatrixTranslation(result.x, result.y, result.z);
}

inline XMMATRIX InterpolateRotation(const NodeAnimation& channel, float time)
{
    if (channel.rotations.size() == 1)
    {
        XMVECTOR q = XMLoadFloat4(&channel.rotations[0].rotation);
        return XMMatrixRotationQuaternion(q);
    }

    int index = 0;
    for (int i = 0; i < channel.rotations.size() - 1; i++)
    {
        if (time < channel.rotations[i + 1].timeStamp)
        {
            index = i;
            break;
        }
    }

    const auto& r0 = channel.rotations[index];
    const auto& r1 = channel.rotations[index + 1];

    float t = (time - r0.timeStamp) / (r1.timeStamp - r0.timeStamp);

    XMVECTOR q0 = XMLoadFloat4(&r0.rotation);
    XMVECTOR q1 = XMLoadFloat4(&r1.rotation);

    XMVECTOR q = XMQuaternionSlerp(q0, q1, t);

    return XMMatrixRotationQuaternion(q);
}

inline XMMATRIX InterpolateScale(const NodeAnimation& channel, float time)
{
    if (channel.scales.size() == 1)
    {
        return XMMatrixScaling(
            channel.scales[0].scale.x,
            channel.scales[0].scale.y,
            channel.scales[0].scale.z
        );
    }

    int index = 0;
    for (int i = 0; i < channel.scales.size() - 1; i++)
    {
        if (time < channel.scales[i + 1].timeStamp)
        {
            index = i;
            break;
        }
    }

    const auto& s0 = channel.scales[index];
    const auto& s1 = channel.scales[index + 1];

    float t = (time - s0.timeStamp) / (s1.timeStamp - s0.timeStamp);

    XMVECTOR v0 = XMLoadFloat3(&s0.scale);
    XMVECTOR v1 = XMLoadFloat3(&s1.scale);

    XMVECTOR scale = XMVectorLerp(v0, v1, t);

    XMFLOAT3 result;
    XMStoreFloat3(&result, scale);

    return XMMatrixScaling(result.x, result.y, result.z);
}

inline XMMATRIX GetLocalTransform(const NodeAnimation& channel, float time)
{
    XMMATRIX T = InterpolatePosition(channel, time);
    XMMATRIX R = InterpolateRotation(channel, time);
    XMMATRIX S = InterpolateScale(channel, time);

    return S * R * T;
}
