#pragma once

#include <memory>

// Forward declaration to avoid circular include with GameObject
class GameObject;

class Component
{
public:
    GameObject* owner = nullptr;

    Component(GameObject* owner) : owner(owner) {}
    virtual ~Component() = default;

    virtual void Update(float dt) {}
};


class AnimationComponent : public Component
{
public:
    void Update(float dt) override
    {
        // ボーン更新
    }
};
