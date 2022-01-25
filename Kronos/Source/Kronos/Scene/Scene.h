#pragma once

#include <memory>
#include <vector>
#include <concepts>
#include <utility>

#include "Kronos/Scene/Entity.h"

namespace Kronos
{
	class Scene
	{
	public:
		template<std::derived_from<Entity> T, typename... Args>
		inline T& CreateEntity(Args&&... args)
		{
			return static_cast<T&>(*m_Entities.emplace_back(std::make_unique<T>(std::forward<Args>(args)...)));
		}

		void DeleteEntity(const Entity& entity)
		{
			
		}

		const std::vector<std::unique_ptr<Entity>>& GetRegistry() const { return m_Entities; }

	private:
		std::vector<std::unique_ptr<Entity>> m_Entities;
	};
}
