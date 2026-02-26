#pragma once

#include <cstdint>
#include <vector>

using Entity = uint32_t;

constexpr Entity INVALID_ENTITY = 0;


class EntityManager
{
public:
    Entity Create()
    {
        if (!freeList.empty())
        {
            Entity e = freeList.back();
            freeList.pop_back();
            return e;
        }

        return nextId++;
    }

    void Destroy(Entity e)
    {
        freeList.push_back(e);
    }

private:
    Entity nextId = 1;
    std::vector<Entity> freeList;
};