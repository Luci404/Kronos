/*
RegisterIntegrant:
	const char* typeName = typeid(T).name();
	assert(m_IntegrantRegistries.contains(typeName)); // TODO: Error message.
	m_IntegrantPools.insert({ typeName, std::make_shared<ComponentArray<T>>() });
*/

#include <unordered_map>
#include <memory>
#include <cassert>
#include <concepts>

// Integrant Core
#define INTEGRANT_REGISTRY_SIZE 1024

class IIntegrant {};

class IIntegrantPool {};
template<std::derived_from<IIntegrant> T>
class IntegrantPool : public IIntegrantPool
{
private:
    std::array<T, INTEGRANT_REGISTRY_SIZE> m_IntegrantArray;
};

class IntegrantRegistry
{
public:
	template<std::derived_from<IIntegrant> T>
	void RegisterIntegrant() {}
    
    template<std::derived_from<IIntegrant> T, typename... Args>
    T* CreateIntegrant(Args&&... args) {}

	void DestroyIntegrant( /* ??? */) {}

private:
	std::unordered_map<const char*, std::shared_ptr<IntegrantPool>> m_IntegrantPools{};
};

// Integrant Types
class TransformIntegrant : public IIntegrant {};

class SomeModule : public StaticModule
{
	void Initialize() override
	{
		IntegrantRegistry::RegisterIntegrant<TransformIntegrant>();
	}
};

// Entity Core
class IEntity {};

class MoveableEntity : public IEntity
{
public:
	void Initialize() override
	{
		m_TransformIntegrant = IntegrantRegistry::CreateIntegrant<TransformIntegrant>(this /* ? */);
		// Consider adding integrants with a member function on the Entity class, that way, cleanup could be automatic.
	}

private:
	TransformIntegrant* m_TransformIntegrant;
};

// Renderer Module
class MeshRendererIntegrant : public IIntegrant {};

class RendererModule : public StaticModule
{
public:
	void Initialize() override
	{
		IntegrantRegistry::RegisterIntegrant<MeshRendererIntegrant>();
	}
};

class RendererSystem : public ISystem 
{
public:
	void SomeFunction()
	{
		// Look at EnTT, also, idk how lambdas work....
		IntegrantRegistry::Each<MeshRendererIntegrant>([](MeshRendererIntegrant& meshRendererIntegrant) {
			/* Do logic */
			}
		);
	}
};