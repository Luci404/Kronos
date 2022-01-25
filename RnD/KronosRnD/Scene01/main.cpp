#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <concepts>
#include <utility>
#include <type_traits>
#include <unordered_map>

// https://austinmorlan.com/posts/entity_component_system/
// Watch this video about data-oriented design: https://www.youtube.com/watch?v=rX0ItVEVjHc&t=1941s&ab_channel=CppCon

// GLOBAL MEMORY POOLS?

#pragma region Integrant Core
#define INTEGRANT_REGISTRY_SIZE 1024

class IIntegrant {};

class IIntegrantRegistry {};
template<std::derived_from<IIntegrant> T>
class IntegrantRegistry : public IIntegrantRegistry
{
private:
    std::array<T, INTEGRANT_REGISTRY_SIZE> m_IntegrantRegistry;
};

class 
#pragma endregion

#pragma region Integrants
class TransformIntegrant : public IIntegrant
{
public:
    TransformIntegrant() : m_PositionX(0.0f), m_PositionY(0.0f), m_PositionZ(0.0f) 
    {
    }

    void SetPosition(float positionX, float positionY, float positionZ)
    {
        m_PositionX = positionX;
        m_PositionY = positionY;
        m_PositionZ = positionZ;
    }

private:
    float m_PositionX, m_PositionY, m_PositionZ;
};
#pragma endregion

class Entity
{
public:
    Entity(const std::string& name) : m_Name(name) {}
    virtual const std::string& GetName() const { return m_Name; }

protected:
    std::string m_Name;
};

class Scene
{
public:
    template<std::derived_from<Entity> T, typename... Args>
    T& CreateEntity(Args&&... args)
    {
        return static_cast<T&>(*m_Entities.emplace_back(std::make_unique<T>(std::forward<Args>(args)...)));
    }

    void DestroyEntity(Entity* entity)
    {
        for (size_t i = 0; i < m_Entities.size(); i++)
        {
            if (m_Entities[i].get() == entity)
            {
                m_Entities.erase(m_Entities.begin() + i);
                return;
            }
        }
     
        std::cout << "Failed to delete entity!\n";
    }

    template<std::derived_from<Entity> T, typename... Args>
    T& CreateIntegrant(Args&&... args)
    {
        const std::string typeName = typeid(T).name()
        // return static_cast<T&>(*m_Entities.emplace_back(std::make_unique<T>(std::forward<Args>(args)...)));
    }

    const std::vector<std::unique_ptr<Entity>>& GetRegistry() const
    {
        return m_Entities;
    }

private:
    std::vector<std::unique_ptr<Entity>> m_Entities;
    std::unordered_map<const std::string, std::shared_ptr<IIntegrantRegistry>> m_IntegrantRegistries;
};

class MoveableEntity : public Entity
{
public:
    MoveableEntity(const std::string& name) : Entity(name)  { }
};

int main(int argc, char* argv[])
{
    Scene scene;
    scene.CreateEntity<Entity>("Entity01");
    scene.CreateEntity<Entity>("Entity02");
    scene.CreateEntity<Entity>("Entity02");
    MoveableEntity& moveableEntity = scene.CreateEntity<MoveableEntity>("MoveableEntity01");
    for (const std::unique_ptr<Entity>& entity : scene.GetRegistry())
    {
        std::cout << entity->GetName() << "\n";
    }
    std::cout << '\n';

    scene.DestroyEntity(&moveableEntity);
    for (const std::unique_ptr<Entity>& entity : scene.GetRegistry())
    {
        std::cout << entity->GetName() << "\n";
    }

    return 0;
}