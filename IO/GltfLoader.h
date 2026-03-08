#pragma once
#include <memory>
#include <string>
#include <d3d11.h>
#include <assimp/scene.h>

#include "Graphics/Model.h"
#include "IO/TextureLoader.h"

class GltfLoader
{
public:
    GltfLoader(ID3D11Device* device);
    std::shared_ptr<ModelBase> Load(const std::string& path);


private:
    TextureLoader textureLoader;
    ID3D11Device* device = nullptr; // ★ メンバとして保持
    std::unique_ptr<Node> ProcessNode(aiNode* node);
    Mesh ProcessMesh(aiMesh* mesh);
    Material ProcessMaterial(aiMaterial* aimat);
    void ProcessBones(aiMesh* mesh, SkinnedModel* model, int meshIndex);
    void ProcessAnimations(const aiScene* scene, SkinnedModel* model);
    void LinkBonesToNodes(SkinnedModel* model);
};
