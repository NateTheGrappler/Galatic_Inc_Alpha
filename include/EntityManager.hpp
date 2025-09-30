//DONE
#pragma once
#include <map>
#include <vector>
#include <memory>
#include "Entity.hpp"

using EntityVec = std::vector<std::shared_ptr<Entity>>;

class EntityManager
{
    EntityVec                          m_entities;
    EntityVec                          m_entitiesToAdd;
    std::map<std::string, EntityVec>   m_entityMap;
    size_t                             m_totalEntities = 0;

    void removeDeadEntites(EntityVec& vec)
    {
        auto newEnd = std::remove_if(
            vec.begin(), vec.end(),
            [](const std::shared_ptr<Entity>& entity)
            {
                return !entity->isActive();
            }
        );
        vec.erase(newEnd, vec.end());
    }
public:
    EntityManager() = default;

    void update()
    {
        removeDeadEntites(m_entities);

        for (auto& [tag, entityVec] : m_entityMap)
        {
            removeDeadEntites(entityVec);
        }
        for (auto& entity : m_entitiesToAdd)
        {
            m_entities.push_back(entity);
        }
        m_entitiesToAdd.clear();
    }

    std::shared_ptr<Entity> addEntity(const std::string tag)
    {
        auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
        m_entitiesToAdd.push_back(entity);

        if (m_entityMap.find(tag) == m_entityMap.end()) { m_entityMap[tag] = EntityVec(); }
        m_entityMap[tag].push_back(entity);
        return entity;
    }

    const EntityVec& getEntities()
    {
        return m_entities;
    }
    const EntityVec& getEntities(const std::string& tag)
    {
        if (m_entityMap.find(tag) == m_entityMap.end()) { m_entityMap[tag] = EntityVec(); }
        return m_entityMap[tag];
    }
    const std::map<std::string, EntityVec>& getEntityMap()
    {
        return m_entityMap;
    }
};