#include "GameObject.h"

void GameObject::Update(float dt)
{
    if (!active) return;

    // ---- 親ノードにアタッチされている場合 ----
    if (parentObject && parentObject->model)
    {
        Node* node = parentObject->model->FindNode(parentNodeName);
        if (node)
        {
            // 親ノードの globalTransform をそのまま使う
            transform.world = node->globalTransform;
        }
        else
        {
            // ノードが見つからない場合は通常更新
            transform.Update(DirectX::XMMatrixIdentity());
        }
    }
    else
    {
        // 通常の Transform 更新
        transform.Update(DirectX::XMMatrixIdentity());
    }

    if (auto skinned = dynamic_cast<SkinnedModel*>(model.get()))
    {
        skinned->UpdateAnimation(dt);
    }

    // ---- コンポーネント更新 ----
    for (auto& c : components)
        c->Update(dt);
}

void GameObject::Draw(Renderer& renderer)
{
    if (!visible || !model) return;

    renderer.DrawModel(*model, transform);
}

// ---- アタッチ機能 ----
void GameObject::AttachTo(GameObject* parent, const std::string& nodeName)
{
    parentObject = parent;
    parentNodeName = nodeName;
}

void GameObject::Detach()
{
    parentObject = nullptr;
    parentNodeName.clear();
}
