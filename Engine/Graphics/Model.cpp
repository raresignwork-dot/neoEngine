#include "Graphics/Model.h"
#include <iostream>
#include "Scene/Node.h"
#include "Animation/Animation.h"


void ModelBase::DebugPrint()
{
    std::cout << "=== Model Debug Info ===\n";

    // --- Mesh 情報 ---
    std::cout << "Meshes: " << meshes.size() << "\n";
    for (size_t i = 0; i < meshes.size(); i++)
    {
        std::cout << "  Mesh[" << i << "] vertices=" << meshes[i].vertices.size()
            << " indices=" << meshes[i].indices.size() << "\n";
    }

    // --- Material 情報 ---
    std::cout << "Materials: " << materials.size() << "\n";
    for (size_t i = 0; i < materials.size(); i++)
    {
        const auto& m = materials[i];
        std::cout << "  Material[" << i << "]\n";
        std::cout << "    baseColor: ("
            << m.baseColor.x << ", "
            << m.baseColor.y << ", "
            << m.baseColor.z << ", "
            << m.baseColor.w << ")\n";

        if (!m.baseColorTex.path.empty())
            std::cout << "    baseColorTex: " << m.baseColorTex.path << "\n";
        if (!m.normalTex.path.empty())
            std::cout << "    normalTex: " << m.normalTex.path << "\n";
        if (!m.metallicRoughnessTex.path.empty())
            std::cout << "    metallicRoughnessTex: " << m.metallicRoughnessTex.path << "\n";
        if (!m.diffuseTex.path.empty())
            std::cout << "    diffuseTex: " << m.diffuseTex.path << "\n";
    }

    // --- Node 階層 ---
    std::cout << "Node Hierarchy:\n";
    if (rootNode)
        rootNode->DebugPrint(0);
    else
        std::cout << "  (no root node)\n";
}



void SkinnedModel::UpdateAnimation(float dt)
{
    Animation& anim = animations[currentAnimation];

    currentTime += dt * anim.ticksPerSecond;
    currentTime = fmod(currentTime, anim.duration);

    for (auto& channel : anim.channels)
    {
        Node* node = FindNode(channel.nodeName);
        if (!node) continue;

        node->localTransform = GetLocalTransform(channel, currentTime);
    }

    rootNode->UpdateGlobal(XMMatrixIdentity());
}

void SkinnedModel::CalculateFinalMatrices()
{
    finalMatrices.resize(boneInfo.size());

    for (auto& [name, bone] : boneInfo)
    {
        Node* node = FindNode(name);
        if (!node) continue;

        finalMatrices[bone.id] = node->globalTransform * bone.offsetMatrix;
    }
}


void SkinnedModel::Draw(XMMATRIX world)
{
    // アニメーション更新後に finalMatrices を計算
    CalculateFinalMatrices();

    // Renderer 側でスキニング用シェーダをセットする想定
    // finalMatrices を GPU に送る（定数バッファ or StructuredBuffer）

    for (auto& mesh : meshes)
    {
        // スキン付き描画
        // mesh.DrawSkinned(finalMatrices, world);
    }
}

void SkinnedModel::DebugPrint()
{
    ModelBase::DebugPrint();

    std::cout << "Bones: " << boneInfo.size() << "\n";
    for (auto& [name, info] : boneInfo)
    {
        std::cout << "  Bone: " << name << " id=" << info.id << "\n";
    }

    std::cout << "Animations: " << animations.size() << "\n";
    for (size_t i = 0; i < animations.size(); i++)
    {
        std::cout << "  Animation[" << i << "] duration="
            << animations[i].duration
            << " tps=" << animations[i].ticksPerSecond << "\n";
    }
}

