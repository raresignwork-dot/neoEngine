#include "Scene.h"


Scene::Scene()
{
    
}

Scene::~Scene()
{
    //Unload();
}

void Scene::Update(SceneContext& ctx)
{

    for (auto& obj : objects)
    {
        obj->transform.Update(XMMatrixIdentity());
    }

    for (auto& obj : objects)
    {
        if (obj->active)
            obj->Update(ctx.deltaTime);
    }
}

void Scene::Draw(SceneContext& ctx)
{
    auto renderer = ctx.renderer; // shared_ptr<Renderer>

    for (auto& obj : objects)
    {
        if (obj->visible && obj->model)
        {
            renderer->DrawModel(*obj->model, obj->transform);
        }
    }
}