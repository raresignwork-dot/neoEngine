#pragma once
#include <memory>
#include "Scene/Scene.h"

class SceneManager {
public:
    SceneManager() = default;
    ~SceneManager() = default;

    template <typename T>
    void ChangeScene()
    {
        m_pNextScene = std::make_unique<T>();
    }

    void Update(SceneContext& ctx);
    void Draw(SceneContext& ctx);
    void Draw2D(SceneContext& ctx);

private:
    std::unique_ptr<Scene> m_pCurrentScene;
    std::unique_ptr<Scene> m_pNextScene;
};
