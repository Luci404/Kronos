#include <string>
#include <memory>
#include <vector>
#include <iostream>
#include <concepts>
#include <utility>

class Entity
{
public:
    Entity(const std::string& name) 
        : m_Name(name)
    { 
    }

    virtual const std::string& GetName() const { return m_Name; }

protected:
    std::string m_Name;
};

class StaticMesh : public Entity
{
public:
    StaticMesh() 
        : Entity("StaticMesh")
    { 
    }
};

class Scene
{
public:
    template<std::derived_from<Entity> T, typename... Args>
    T& CreateEntity(Args&&... args)
    {
        return static_cast<T&>(*m_Entities.emplace_back(std::make_unique<T>(std::forward<Args>(args)...)));
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

    scene.CreateEntity<Entity>("Entity 01");
    scene.CreateEntity<StaticMesh>();

    scene.Print();

    return 0;
}