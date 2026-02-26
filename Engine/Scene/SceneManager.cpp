#include "SceneManager.h"

void SceneManager::Update(SceneContext& ctx)
{
    if (m_pNextScene)
    {
        // Try to load the next scene first. If loading succeeds, replace the current scene.
        // This prevents losing the current scene if the next scene fails to load.
        std::unique_ptr<Scene> candidate = std::move(m_pNextScene);
        if (candidate->Load(ctx)) {
            if (m_pCurrentScene) {
                m_pCurrentScene->Unload();
            }
            m_pCurrentScene = std::move(candidate);
        }
        else {
            // Loading failed: keep current scene and discard candidate
            candidate.reset();
            OutputDebugStringA("SceneManager::Update: next scene Load() failed\n");
        }
    }

    if (m_pCurrentScene)
        m_pCurrentScene->Update(ctx);
}

void SceneManager::Draw(SceneContext& ctx)
{
    if (m_pCurrentScene)
        m_pCurrentScene->Draw(ctx);
}

void SceneManager::Draw2D(SceneContext& ctx)
{
    if (m_pCurrentScene)
        m_pCurrentScene->Draw2D(ctx);
}
