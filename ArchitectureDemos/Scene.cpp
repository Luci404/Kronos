#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <concepts>

class Entity
{
public:
    Entity() { m_Name = "Entity"; }

    virtual const std::string& GetName() const { return m_Name; }

protected:
    std::string m_Name;
};

class StaticMesh : public Entity 
{
public:
    StaticMesh() { m_Name = "StaticMesh"; }
};

class Scene
{
public:
    template<std::derived_from<Entity> T, typename... Args>
    T& CreateEntity(Args&&... args)
    {
        return emplace_back(std::make_unique<T>(args));
    }

    void Print()
    {
        for (std::unique_ptr<Entity>& entity : m_Entities)
        {
            std::cout << entity->GetName() << "\n";
        }
    }

private:
    std::vector<std::unique_ptr<Entity>> m_Entities;
};

int main(int argc, char* argv[])
{
    Scene scene;
    
    scene.CreateEntity<Entity>();
    scene.CreateEntity<StaticMesh>();

    scene.Print();

    return 0;
}