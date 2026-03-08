#include "IO/GltfLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <DirectXMath.h>

#include "../Graphics/Model.h"

using namespace DirectX;

GltfLoader::GltfLoader(ID3D11Device* device)
    : device(device), textureLoader(device)
{
}


std::shared_ptr<ModelBase> GltfLoader::Load(const std::string& path)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_LimitBoneWeights
    );


    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("Assimp error: %s\n", importer.GetErrorString());
        return nullptr;
    }

    bool hasBones = false;
    for (unsigned i = 0; i < scene->mNumMeshes; i++)
        if (scene->mMeshes[i]->HasBones())
            hasBones = true;

    std::shared_ptr<ModelBase> model;

    if (hasBones)
        model = std::make_shared<SkinnedModel>();
    else
        model = std::make_shared<StaticModel>();


    model->rootNode = ProcessNode(scene->mRootNode);

    model->meshes.reserve(scene->mNumMeshes);
    for (unsigned i = 0; i < scene->mNumMeshes; i++)
    {
        aiMesh* aimesh = scene->mMeshes[i];
        Mesh mesh = ProcessMesh(aimesh);
        model->meshes.push_back(mesh);

        if (hasBones)
            ProcessBones(aimesh, (SkinnedModel*)model.get(), i);
    }

    model->materials.reserve(scene->mNumMaterials);
    for (unsigned i = 0; i < scene->mNumMaterials; i++)
    {
        aiMaterial* aimat = scene->mMaterials[i];
        Material mat = ProcessMaterial(aimat);

        model->materials.push_back(mat);
    }

    if (hasBones)
    {
        ProcessAnimations(scene, (SkinnedModel*)model.get());
        LinkBonesToNodes((SkinnedModel*)model.get());
    }

    // GPU バッファ作成
    for (auto& mesh : model->meshes)
    {
        mesh.CreateBuffers(device);   // ← これが必須
    }



    return model;
}

std::unique_ptr<Node> GltfLoader::ProcessNode(aiNode* ainode)
{
    auto node = std::make_unique<Node>();
    node->name = ainode->mName.C_Str();

    //XMMATRIX m = XMMatrixTranspose(XMLoadFloat4x4((XMFLOAT4X4*)&ainode->mTransformation));
    //node->localTransform = m;

    for (unsigned i = 0; i < ainode->mNumMeshes; i++)
        node->meshIndices.push_back(ainode->mMeshes[i]);

    for (unsigned i = 0; i < ainode->mNumChildren; i++)
    {
        auto child = ProcessNode(ainode->mChildren[i]);
        child->parent = node.get();
        node->children.push_back(std::move(child));
    }

    return node;
}

Mesh GltfLoader::ProcessMesh(aiMesh* aimesh)
{
    Mesh mesh;

    mesh.vertices.resize(aimesh->mNumVertices);
    mesh.materialIndex = aimesh->mMaterialIndex;

    for (unsigned i = 0; i < aimesh->mNumVertices; i++)
    {
        auto& v = mesh.vertices[i];

        v.position = { aimesh->mVertices[i].x, aimesh->mVertices[i].y, aimesh->mVertices[i].z };
        v.normal = { aimesh->mNormals[i].x,  aimesh->mNormals[i].y,  aimesh->mNormals[i].z };

        if (aimesh->mTextureCoords[0])
            v.uv = { aimesh->mTextureCoords[0][i].x, aimesh->mTextureCoords[0][i].y };
        else
            v.uv = { 0, 0 };
    }

    for (unsigned i = 0; i < aimesh->mNumFaces; i++)
    {
        aiFace face = aimesh->mFaces[i];
        for (unsigned j = 0; j < face.mNumIndices; j++)
            mesh.indices.push_back(face.mIndices[j]);
    }

    return mesh;
}

Material GltfLoader::ProcessMaterial(aiMaterial* aimat)
{
    Material mat;

    aiColor4D base;
    if (AI_SUCCESS == aimat->Get(AI_MATKEY_BASE_COLOR, base))
        mat.baseColor = XMFLOAT4(base.r, base.g, base.b, base.a);

    auto LoadTex = [&](aiTextureType type, TextureInfo& out)
    {
        if (aimat->GetTextureCount(type) > 0)
        {
            aiString str;
            aimat->GetTexture(type, 0, &str);
            out.path = str.C_Str();
        }
    };

    LoadTex(aiTextureType_BASE_COLOR, mat.baseColorTex);
    LoadTex(aiTextureType_DIFFUSE, mat.diffuseTex);
    LoadTex(aiTextureType_NORMALS, mat.normalTex);

    if (!mat.baseColorTex.path.empty())
        mat.baseColorTexture = textureLoader.Load(mat.baseColorTex.path);

    return mat;
}

void GltfLoader::ProcessBones(aiMesh* aimesh, SkinnedModel* model, int meshIndex)
{
    for (size_t i = 0; i < aimesh->mNumBones; i++)
    {
        aiBone* aibone = aimesh->mBones[i];
        std::string name = aibone->mName.C_Str();

        // 新しいボーンなら登録
        if (model->boneInfo.find(name) == model->boneInfo.end())
        {
            BoneInfo info;
            info.name = name;
            info.id = static_cast<int>(model->boneInfo.size());

            XMMATRIX offset = XMMatrixTranspose(
                XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&aibone->mOffsetMatrix))
            );
            info.offsetMatrix = offset;

            model->boneInfo[name] = info;
        }

        int boneID = model->boneInfo[name].id;

        // 頂点ウェイトを登録
        for (size_t w = 0; w < aibone->mNumWeights; w++)
        {
            unsigned vertexID = static_cast<unsigned>(aibone->mWeights[w].mVertexId);
            float weight = aibone->mWeights[w].mWeight;

            model->meshes[meshIndex].AddBoneWeight(vertexID, boneID, weight);
        }
    }
}



void GltfLoader::ProcessAnimations(const aiScene* scene, SkinnedModel* model)
{
    for (unsigned a = 0; a < scene->mNumAnimations; a++)
    {
        aiAnimation* aianim = scene->mAnimations[a];

        Animation anim;
        anim.duration = (float)aianim->mDuration;
        anim.ticksPerSecond = (float)(aianim->mTicksPerSecond != 0 ? aianim->mTicksPerSecond : 30.0f);

        model->animations.push_back(anim);
    }
}

void GltfLoader::LinkBonesToNodes(SkinnedModel* model)
{
    for (auto& [name, bone] : model->boneInfo)
    {
        Node* node = model->FindNode(name);
        if (!node)
        {
            printf("Warning: Bone '%s' not found in Node hierarchy\n", name.c_str());
        }
    }
}
