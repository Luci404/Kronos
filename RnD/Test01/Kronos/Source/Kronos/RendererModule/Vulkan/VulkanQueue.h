#pragma once

#include "Kronos/Core/Assert.h"

#include "Kronos/RendererModule/Vulkan/VulkanDevice.h"

#define VK_USE_PLATFORM_WIN32_KHR 
#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace Kronos
{
	class VulkanQueue
	{
	public:
		VulkanQueue(VulkanDevice& device, uint32_t familyIndex, VkQueueFamilyProperties properties, VkBool32 supportPresent, uint32_t index)
			: m_Device{ device }, m_FamilyIndex{ familyIndex }, m_Index{index}, m_SupportPresent{ supportPresent }, m_Properties{ properties }
		{
			vkGetDeviceQueue(m_Device.GetHandle(), m_FamilyIndex, m_Index, &m_Handle);
		}

		VulkanQueue(const VulkanQueue&) = delete;
		VulkanQueue(VulkanQueue&&);
		VulkanQueue& operator=(const VulkanQueue&) = delete;
		VulkanQueue& operator=(VulkanQueue&&) = delete;


		VkResult Submit(const std::vector<VkSubmitInfo>& submitInfos, VkFence fence) const
		{
			return vkQueueSubmit(m_Handle, static_cast<uint32_t>(submitInfos.size()), submitInfos.data(), fence);
		}

		VkResult Present(const VkPresntInfoKHR presentInfo) const
		{
			KRONOS_CORE_ASSERT(m_SupportPresent, "Failed to present, the queue does not have present support!");
			return vkQueuePresntKHR(m_Handle, &presnetInfo);
		}

		const VkQueue& GetHandle() const { return m_Device; }

		const Device& GetDevice() const { return m_Device; }
		const uint32_t& GetFamilyIndex() const { return m_FamilyIndex; }
		const uint32_t& GetIndex() const { return m_Index; }
		const VkQueueFamilyProperties& GetProperties() const { return m_Properties; }
		const VkBool32& SupportPresent() const { return m_SupportPresent; }

	private:
		VkQueue m_Handle;
		
		Device& m_Device;
		uint32_t m_FamilyIndex;
		uint32_t m_Index;
		VkQueueFamilyProperties m_Properties{};
		VkBool32 m_SupportPresent;
	};
}