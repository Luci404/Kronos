#pragma once

#include "Kronos/Core/Assert.h"

#define VK_USE_PLATFORM_WIN32_KHR 
#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace Kronos
{
	class VulkanDevice;

	class VulkanQueue
	{
	public:
		VulkanQueue(VulkanDevice& device, uint32_t familyIndex, VkQueueFamilyProperties properties, VkBool32 supportPresent, uint32_t index);

		VulkanQueue(const VulkanQueue&) = delete;
		VulkanQueue(VulkanQueue&& other);
		VulkanQueue& operator=(const VulkanQueue&) = delete;
		VulkanQueue& operator=(VulkanQueue&&) = delete;

		VkResult Submit(const std::vector<VkSubmitInfo>& submitInfos, VkFence fence) const;

		VkResult Present(const VkPresentInfoKHR presentInfo) const;

		const VulkanDevice& GetDevice() const { return m_Device; }
		const VkQueue& GetHandle() const { return m_Handle; }
		uint32_t GetFamilyIndex() const { return m_FamilyIndex; }
		uint32_t GetIndex() const { return m_Index; }
		const VkQueueFamilyProperties& GetProperties() const { return m_Properties; }
		VkBool32 SupportPresent() const { return m_SupportPresent; }

	private:
		VulkanDevice& m_Device;
		VkQueue m_Handle;
		uint32_t m_FamilyIndex;
		uint32_t m_Index;
		VkQueueFamilyProperties m_Properties{};
		VkBool32 m_SupportPresent;
	};
}