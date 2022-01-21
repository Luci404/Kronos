#include <string>
#include <memory>
#include <vector>

class Entity
{

};

// class EntityStack {};

class StaticMesh : public Entity 
{
};

class Scene
{
public:
    template<class T>
    T& CreateEntity(const std::string& name)
    {
        m_Entities = T()
    }
private:
    std::vector<Entity*> m_Entities;
};