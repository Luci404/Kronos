#include <unordered_map>
#include <memory>
#include <cassert>
#include <concepts>

#define KRONOS_CORE_ASSERT(x, ...) assert((x))

#define INTEGRANT_REGISTRY_SIZE 1024

#pragma region Integrant
class IIntegrant {};
#pragma endregion 

#pragma region IntegrantHandle
template<std::derived_from<IIntegrant> T>
class IntegrantHandle
{
public:
	friend class IntegrantRegistry;

public:
	virtual ~IntegrantHandle()
	{
		m_Count--;
	}

	static uint16_t GetCount()
	{
		return m_Count;
	}

	T* operator->() const noexcept 
	{
		return *IntegrantRegistry::GetIntegrant<T>(this);
	}

private:
	IntegrantHandle(uint16_t identifier)
		: m_Identifier(identifier)
	{
		m_Count++;
	}

private:
	static uint16_t m_Count;
	
	uint16_t m_Identifier;
};
#pragma endregion

#pragma region IntegrantPool
class IIntegrantPool {};

template<std::derived_from<IIntegrant> T>
class IntegrantPool final : public IIntegrantPool
{
private: 
	IntegrantHandle<T> Insert(T integrant)
	{
		// Find available integrant identifier/index.
		size_t integrantIndex = m_Size;

		// Add the integrant to the integrant array, and integrant identifier array.
		m_IntegrantArray[integrantIndex] = integrant;
		
		IntegrantHandle<T> integrantHandle = IntegrantHandle<T>(m_Size);
		m_IntegrantHandleToIndexMap[integrantHandle] = integrantIndex;
		m_IndexToIntegrantHandleMap[integrantIndex] = integrantHandle;

		m_Size++;

		return integrantHandle;
	}

	void Remove(IntegrantHandle<T> integrantHandle)
	{
		// TODO: Assert
		size_t integrantIndex = m_IntegrantHandleToIndexMap[integrantHandle];

		// Copy the integrant at end of the array to the deleted element's place to maintain memory density.
		size_t indexOfRemovedIntegrant = m_IntegrantHandleToIndexMap[integrantHandle];
		size_t indexOfLastIntegrant = m_Size - 1;
		m_IntegrantArray[indexOfRemovedIntegrant] = m_IntegrantArray[indexOfLastIntegrant];

		// Update map to point to the new index of the moved integrant, and remove the deleted integrant's identifier.
		IntegrantHandle<T> integrantHandleOfLastElement = m_IndexToIntegrantHandleMap[indexOfLastIntegrant];
		m_IntegrantHandleToIndexMap[integrantHandleOfLastElement] = indexOfRemovedIntegrant;
		m_IndexToIntegrantHandleMap[indexOfRemovedIntegrant] = integrantHandleOfLastElement;

		m_IntegrantHandleToIndexMap.erase(integrantHandleOfLastElement);
		m_IndexToIntegrantHandleMap.erase(indexOfRemovedIntegrant);

		--m_Size;
	}

	T& Get(IntegrantHandle<T> integrantHandle)
	{
		return m_IntegrantArray[m_IntegrantHandleToIndexMap[integrantHandle]];
	}
private:
    std::array<T, INTEGRANT_REGISTRY_SIZE> m_IntegrantArray;
	std::unordered_map<IntegrantHandle<T>, size_t> m_IntegrantHandleToIndexMap;
	std::unordered_map<size_t, IntegrantHandle<T>> m_IndexToIntegrantHandleMap;

	size_t m_Size;
};
#pragma endregion

#pragma region IntegrantRegistry
class IntegrantRegistry
{
public:
	template<std::derived_from<IIntegrant> T>
	friend class IntegrantHandle;

public:
	template<std::derived_from<IIntegrant> T>
	static void RegisterIntegrant() 
	{
		const char* typeName = typeid(T).name();
		
		KRONOS_CORE_ASSERT(m_IntegrantPools.contains(typeName), "Failed to register integrant: An integrant type with typename '%s' has already been registered.", typeName);
		
		m_IntegrantPools.insert({ typeName, std::make_shared<IntegrantPool<T>>() });
	}
    
    template<std::derived_from<IIntegrant> T, typename... Args>
	static IntegrantHandle<T> CreateIntegrant(Args&&... args)
	{
		return GetIntegrantPool<T>()->Insert(T(args));
	}

	template<std::derived_from<IIntegrant> T>
	static void DestroyIntegrant(IntegrantHandle<T> integrantHandle)
	{
		// TODO: Invalidate handles.
		GetIntegrantPool<T>()->Remove(integrantHandle);
	}

	template<std::derived_from<IIntegrant> T>
	static T& GetIntegrant(IntegrantHandle<T> integrantHandle)
	{
		GetIntegrantPool<T>()->Get(integrantHandle);
	};

	template<std::derived_from<IIntegrant> T, typename F>
	static void EachIntegrant(F& lambda) {};

private:
	IntegrantRegistry() {}

	template<std::derived_from<IIntegrant> T>
	std::shared_ptr<T> GetIntegrantPool()
	{
		const char* typeName = typeid(T).name();

		KRONOS_CORE_ASSERT(m_IntegrantPools.contains(typeName), "Failed to get integrant pool: Failed find integrant pool of type '%s'.", typeName);

		return std::static_pointer_cast<T>(m_IntegrantPools[typeName]);
	}

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
class ExampleIntegrant : public IIntegrant 
{
public:
	ExampleIntegrant(int integer)
		: ExampleData(integer)
	{
	}

	void DoStuff(int integer)
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
		//IntegrantRegistry::EachIntegrant<ExampleIntegrant>([](ExampleIntegrant& meshRendererIntegrant) { /* Do logic */ });
	}
};

class ExampleEntity : public IEntity
{
public:
	void Initialize() override
	{
		m_ExampleIntegrantHandle = IntegrantRegistry::CreateIntegrant<ExampleIntegrant>(1234);
		m_ExampleIntegrantHandle->DoStuff(1234);
		m_ExampleIntegrantHandle->ExampleData += 1234;

		IntegrantRegistry::DestroyIntegrant<ExampleIntegrant>(m_ExampleIntegrantHandle);
	}

private:
	IntegrantHandle<ExampleIntegrant> m_ExampleIntegrantHandle;
};
#pragma endregion