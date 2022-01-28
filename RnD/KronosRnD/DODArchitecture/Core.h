#pragma once

#include <unordered_map>
#include <memory>
#include <array>
#include <cassert>
#include <utility>
#include <concepts>
#include <iostream>

#define KRONOS_CORE_ASSERT(x, ...) assert((x))
#define INTEGRANT_REGISTRY_SIZE 1024

class IIntegrant {};

template<std::derived_from<IIntegrant> T>
class IntegrantHandle
{
public:
	friend std::hash;

public:
	// The constructor should be private, IntegrantPool should be friend, only allowing construction only from inside of IntegrantPool.
	IntegrantHandle()
		: m_Identifier(m_IncrementalIdentifier)
	{
		std::cout << "Constructor called " << std::endl;
		m_IncrementalIdentifier++;
	}

	bool operator==(const IntegrantHandle&) const = default;

	T* operator->() const noexcept;

	static uint32_t GetCount()
	{
		return m_Count;
	}

private:
	static uint32_t m_IncrementalIdentifier;
	uint32_t m_Identifier;
};

template<std::derived_from<IIntegrant> T>
uint32_t IntegrantHandle<T>::m_IncrementalIdentifier = 0;

template<std::derived_from<IIntegrant> T>
struct std::hash<IntegrantHandle<T>>
{
	std::size_t operator()(const IntegrantHandle<T>& integrantHandle) const
	{
		return std::hash<uint32_t>()(integrantHandle.m_Identifier);
	}
};

// IntegrantPool
class IIntegrantPool {};
template<std::derived_from<IIntegrant> T>
class IntegrantPool final : public IIntegrantPool
{
public:
	IntegrantPool() 
		: m_Size(0)
	{
	}

	IntegrantHandle<T> Insert(T integrant)
	{
		// Find available integrant identifier/index.
		size_t integrantIndex = m_Size;

		// Add the integrant to the integrant array, and integrant identifier array.
		m_IntegrantArray[integrantIndex] = integrant;

		IntegrantHandle<T> integrantHandle = IntegrantHandle<T>();
		m_IntegrantHandleToIndexMap[integrantHandle] = integrantIndex;
		m_IndexToIntegrantHandleMap[integrantIndex] = integrantHandle;

		m_Size++;

		return integrantHandle;
	}

	void Remove(IntegrantHandle<T> integrantHandle)
	{
		// Move the last integrant of the array into the place of hte removed integrant to keep the array memory packed.
		size_t indexOfRemovedIntegrant = m_IntegrantHandleToIndexMap[integrantHandle];
		size_t indexOfLastIntegrant = m_Size - 1;
		m_IntegrantArray[indexOfRemovedIntegrant] = m_IntegrantArray[indexOfLastIntegrant];

		// Update the mappings to accommodate for re-packing of the array.
		IntegrantHandle<T> integrantHandleOfLastElement = m_IndexToIntegrantHandleMap[indexOfLastIntegrant];
		m_IntegrantHandleToIndexMap[integrantHandleOfLastElement] = indexOfRemovedIntegrant;
		// TODO: Fix this... The next line will construct a handle, increment m_IncrementalIdentifier, then override that newly created object with the assignment operator?
		m_IndexToIntegrantHandleMap[indexOfRemovedIntegrant] = integrantHandleOfLastElement;

		// Remove the mappings for the removed integrant.
		m_IntegrantHandleToIndexMap.erase(integrantHandle);
		m_IndexToIntegrantHandleMap.erase(indexOfLastIntegrant);

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

		KRONOS_CORE_ASSERT(!m_IntegrantPools.contains(typeName), "Failed to register integrant: An integrant type with typename '%s' has already been registered.", typeName);

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
	std::cout << "Index: " << m_Identifier << '\n';
	return &IntegrantRegistry::GetIntegrant<T>(*this);
}