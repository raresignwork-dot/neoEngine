#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <memory>
#include <DirectXMath.h>

using namespace DirectX;

class Node
{
public:
    // ---- 基本情報 ----
    std::string name;

    // ローカル変換（親からの相対）
    XMMATRIX localTransform = XMMatrixIdentity();

    // グローバル変換（ワールド空間）
    XMMATRIX globalTransform = XMMatrixIdentity();

    // このノードに紐づくメッシュ
    std::vector<int> meshIndices;

    // 親子関係
    Node* parent = nullptr;
    std::vector<std::unique_ptr<Node>> children;

public:
    Node() = default;
    explicit Node(const std::string& name) : name(name) {}

    // ---- 階層更新 ----
    void UpdateGlobal(const XMMATRIX& parentMatrix)
    {
        globalTransform = localTransform * parentMatrix;

        for (auto& child : children)
            child->UpdateGlobal(globalTransform);
    }

    // ---- 子ノード追加 ----
    Node* AddChild(std::unique_ptr<Node> child)
    {
        child->parent = this;
        Node* ptr = child.get();
        children.push_back(std::move(child));
        return ptr;
    }

    // ---- ノード検索（名前） ----
    Node* Find(const std::string& target)
    {
        if (name == target)
            return this;

        for (auto& child : children)
        {
            if (auto* found = child->Find(target))
                return found;
        }
        return nullptr;
    }

    // ---- ノード検索（パス指定）----
    // 例: "Root/Armature/RightHand"
    Node* FindByPath(const std::string& path)
    {
        size_t pos = path.find('/');
        std::string head = (pos == std::string::npos) ? path : path.substr(0, pos);
        std::string tail = (pos == std::string::npos) ? "" : path.substr(pos + 1);

        if (name != head)
            return nullptr;

        if (tail.empty())
            return this;

        for (auto& child : children)
        {
            if (auto* found = child->FindByPath(tail))
                return found;
        }
        return nullptr;
    }

    // ---- ローカル行列の更新（位置・回転・スケール）----
    void SetLocalTransform(const XMMATRIX& m)
    {
        localTransform = m;
    }

    // ---- グローバル行列を取得 ----
    XMMATRIX GetGlobalTransform() const
    {
        return globalTransform;
    }

    // ---- デバッグ用：階層を表示 ----
    void PrintHierarchy(int depth = 0) const
    {
        for (int i = 0; i < depth; i++) printf("  ");
        printf("%s\n", name.c_str());

        for (auto& child : children)
            child->PrintHierarchy(depth + 1);
    }

    void DebugPrint(int depth = 0) const
    {
        for (int i = 0; i < depth; i++) std::cout << "  ";
        std::cout << "- " << name << "\n";

        // メッシュ情報
        if (!meshIndices.empty())
        {
            for (int i = 0; i < depth + 1; i++) std::cout << "  ";
            std::cout << "meshes: ";
            for (int idx : meshIndices) std::cout << idx << " ";
            std::cout << "\n";
        }

        // ローカル行列
        {
            XMFLOAT4X4 m;
            XMStoreFloat4x4(&m, localTransform);
            for (int i = 0; i < depth + 1; i++) std::cout << "  ";
            std::cout << "local: [" << m._11 << ", " << m._12 << ", ...]\n";
        }

        // 子ノード
        for (auto& child : children)
            child->DebugPrint(depth + 1);
    }


};
