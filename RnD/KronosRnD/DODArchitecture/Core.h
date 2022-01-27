#pragma once

#include <unordered_map>
#include <memory>
#include <array>
#include <cassert>
#include <utility>
#include <concepts>

#define KRONOS_CORE_ASSERT(x, ...) assert((x))
#define INTEGRANT_REGISTRY_SIZE 1024

class IIntegrant {};

template<std::derived_from<IIntegrant> T>
class IntegrantHandle
{
public:
	friend std::hash;

public:
	IntegrantHandle()
	{
	}

	virtual ~IntegrantHandle()
	{
		m_Count--;
	}

	bool operator==(const IntegrantHandle&) const = default;

	static uint16_t GetCount()
	{
		return m_Count;
	}

	T* operator->() const noexcept;

public: // TODO: Make this private, and make IntegrantPool a friend of IntegrantHandle.
	IntegrantHandle(uint16_t identifier)
		: m_Identifier(identifier)
	{
		m_Count++;
	}

private:
	static uint16_t m_Count;

	uint16_t m_Identifier;
};

template<std::derived_from<IIntegrant> T>
struct std::hash<IntegrantHandle<T>>
{
	std::size_t operator()(const IntegrantHandle<T>& integrantHandle) const
	{
		return std::hash<uint16_t>()(integrantHandle.m_Identifier);
	}
};


template<std::derived_from<IIntegrant> T>
uint16_t IntegrantHandle<T>::m_Count = 0;

// IntegrantPool
class IIntegrantPool {};
template<std::derived_from<IIntegrant> T>
class IntegrantPool final : public IIntegrantPool
{
public:
	IntegrantPool() 
	{
	}

	IntegrantHandle<T> Insert(T integrant)
	{
		// Find available integrant identifier/index.
		size_t integrantIndex = m_Size;

		// Add the integrant to the integrant array, and integrant identifier array.
		m_IntegrantArray[integrantIndex] = integrant;

		IntegrantHandle<T> integrantHandle = IntegrantHandle<T>((uint16_t)m_Size);
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

/// IntegrantRegistry
class IntegrantRegistry
{
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
		return GetIntegrantPool<T>()->Insert(T(std::forward<Args>(args)...));
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
		return GetIntegrantPool<T>()->Get(integrantHandle);
	};

	template<std::derived_from<IIntegrant> T, typename F>
	static void EachIntegrant(F& lambda) {};

private:
	IntegrantRegistry() {}

	template<std::derived_from<IIntegrant> T>
	static std::shared_ptr<IntegrantPool<T>> GetIntegrantPool()
	{
		const char* typeName = typeid(T).name();

		KRONOS_CORE_ASSERT(m_IntegrantPools.contains(typeName), "Failed to get integrant pool: Failed find integrant pool of type '%s'.", typeName);

		return std::static_pointer_cast<IntegrantPool<T>>(m_IntegrantPools[typeName]);
	}

private:
	static std::unordered_map<const char*, std::shared_ptr<IIntegrantPool>> m_IntegrantPools;
};
std::unordered_map<const char*, std::shared_ptr<IIntegrantPool>> IntegrantRegistry::m_IntegrantPools = {};

template<std::derived_from<IIntegrant> T>
inline T* IntegrantHandle<T>::operator->() const noexcept
{
	return &IntegrantRegistry::GetIntegrant<T>(*this);
}