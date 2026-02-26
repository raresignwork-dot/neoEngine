#pragma once

#include "Logic/Transform.h"
#include "Graphics/Model.h"
#include "Graphics/Renderer.h"
#include "Logic/Component.h"

#include <vector>
#include <memory>
#include <string>

class GameObject
{
public:
    GameObject() = default;
    ~GameObject() = default;

    GameObject(const GameObject&) = delete;
    GameObject& operator=(const GameObject&) = delete;

    Transform transform;
    std::shared_ptr<ModelBase> model;

    bool active = true;
    bool visible = true;

    // 更新・描画
    void Update(float dt);
    void Draw(Renderer& renderer);

    // コンポーネント追加
    template<typename T, typename... Args>
    T* AddComponent(Args&&... args)
    {
        auto comp = std::make_unique<T>(this, std::forward<Args>(args)...);
        T* ptr = comp.get();
        components.push_back(std::move(comp));
        return ptr;
    }

    // ---- アタッチ機能 ----
    void AttachTo(GameObject* parent, const std::string& nodeName);
    void Detach();

    void SetModel(const std::shared_ptr<ModelBase>& m)
    {
        model = m;
    }

    std::shared_ptr<ModelBase> GetModel() const
    {
        return model;
    }


private:
    std::vector<std::unique_ptr<Component>> components;

    // アタッチ情報
    GameObject* parentObject = nullptr;
    std::string parentNodeName;
};
