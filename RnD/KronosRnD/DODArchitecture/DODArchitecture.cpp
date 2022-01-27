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

#define INTEGRANT_REGISTRY_SIZE 1024

#pragma region Integrant
class IIntegrant
{
public:
	virtual void Initialize() = 0;
};

class TransformIntegrant final : public IIntegrant
{
public:
	void Move(float x, float y, float z) {};
};
#pragma endregion 

#pragma region IntegrantHandle
class IIntegrantHandle
{
public:
	friend class IntegrantRegistry;
protected:
	uint32_t m_Handle;
};

template<std::derived_from<IIntegrant> T>
class IntegrantHandle final : public IIntegrantHandle
{
public:
	T* operator->() const noexcept 
	{
		return *IntegrantRegistry::GetIntegrant<T>();
	}
};
#pragma endregion

#pragma region IntegrantPool
class IIntegrantPool {};

template<std::derived_from<IIntegrant> T>
class IntegrantPool final : public IIntegrantPool
{
private:
    std::array<T, INTEGRANT_REGISTRY_SIZE> m_IntegrantArray;
};
#pragma endregion

#pragma region IntegrantRegistry
class IntegrantRegistry
{
public:
	friend class IIntegrantHandle;

public:
	template<std::derived_from<IIntegrant> T>
	static void RegisterIntegrant() {}
    
    template<std::derived_from<IIntegrant> T, typename... Args>
	static IntegrantHandle<T> CreateIntegrant(Args&&... args) {}

	static void DestroyIntegrant(IIntegrantHandle integrantHandle) {}

	template<std::derived_from<IIntegrant> T>
	static T& GetIntegrant() {};

	template<std::derived_from<IIntegrant> T, typename F>
	static void EachIntegrant(F& lambda) {};

private:
	IntegrantRegistry() {}

private:
	static std::unordered_map<const char*, std::shared_ptr<IIntegrantPool>> m_IntegrantPools;
};

std::unordered_map<const char*, std::shared_ptr<IIntegrantPool>> IntegrantRegistry::m_IntegrantPools = {};
#pragma endregion

#pragma region EngineCore
class IEntity
{
public: 
	virtual void Initialize() = 0;
	virtual void Terminate() = 0;
};

class ISystem 
{
public:
	virtual void Initialize() = 0;
	virtual void Terminate() = 0;
};

class IStaticModule
{
public:
	virtual void Initialize() = 0;
	virtual void Terminate() = 0;
};
#pragma endregion

#pragma region ExampleModule

// Renderer Module
class ExampleIntegrant : public IIntegrant 
{
public:
	ExampleIntegrant(int integer)
		: ExampleData(integer)
	{
	}

	void DoStuff(float integer)
	{
		ExampleData += integer;
	}

public:
	int ExampleData;
};

class ExampleStaticModule : public IStaticModule
{
public:
	void Initialize() override
	{
		IntegrantRegistry::RegisterIntegrant<ExampleIntegrant>();
	}
};

class ExampleSystem : public ISystem
{
public:
	void DoStuff()
	{
		IntegrantRegistry::EachIntegrant<ExampleIntegrant>([](ExampleIntegrant& meshRendererIntegrant) { /* Do logic */ });
	}
};

class ExampleEntity : public IEntity
{
public:
	void Initialize() override
	{
		m_ExampleIntegrant = IntegrantRegistry::CreateIntegrant<ExampleIntegrant>(1234);
		m_ExampleIntegrant->DoStuff(1234);
		m_ExampleIntegrant->ExampleData += 1234;
	}

private:
	IntegrantHandle<ExampleIntegrant> m_ExampleIntegrant;
};
#pragma endregion