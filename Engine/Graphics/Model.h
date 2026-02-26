#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include "../Scene/Node.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/Material.h"
#include "../Shaders/Shader.h"
#include "../Animation/Animation.h"

class ModelBase
{
public:
    virtual ~ModelBase() = default;
    virtual void Draw(DirectX::XMMATRIX world) = 0;
    virtual void DebugPrint();

    std::shared_ptr<Shader> shader;
    std::unique_ptr<Node> rootNode;
    std::vector<Mesh> meshes;
    std::vector<Material> materials;

    Node* FindNode(const std::string& name)
    {
        if (!rootNode) return nullptr;
        return rootNode->Find(name);
    }
};

class StaticModel : public ModelBase
{
public:
    void Draw(DirectX::XMMATRIX world) override {}
};

class SkinnedModel : public ModelBase
{
public:
    std::unordered_map<std::string, BoneInfo> boneInfo;
    std::vector<Animation> animations;
    std::vector<DirectX::XMMATRIX> finalMatrices;

    int currentAnimation = 0;
    float currentTime = 0.0f;

    void UpdateAnimation(float dt);
    void CalculateFinalMatrices();
    void Draw(DirectX::XMMATRIX world) override;
    void DebugPrint();
};
