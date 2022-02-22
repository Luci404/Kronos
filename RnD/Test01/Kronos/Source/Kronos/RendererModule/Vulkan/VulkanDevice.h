#pragma once

#include "Kronos/Core/Assert.h"

#include "Kronos/RendererModule/Vulkan/VulkanPhysicalDevice.h"
#include "Kronos/RendererModule/Vulkan/VulkanQueue.h"

#define VK_USE_PLATFORM_WIN32_KHR 
#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace Kronos
{
	class VulkanQueue;

    class VulkanDevice
    {
    public:
        VulkanDevice(VulkanPhysicalDevice& physicalDevice, VkSurfaceKHR surface, const std::unordered_map<const char *, bool>& requestedExtensions = {});

       	VulkanDevice(const VulkanDevice &) = delete;
    	VulkanDevice(VulkanDevice &&) = delete;
	    VulkanDevice &operator=(const VulkanDevice &) = delete;
	    VulkanDevice &operator=(VulkanDevice &&) = delete;

        VkDevice GetHandle() const { return m_Device; }

		const VulkanQueue& GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const;

		const VulkanQueue& GetQueueByFlags(VkQueueFlags requiredQueueFlags, uint32_t queueIndex) const;

		const VulkanPhysicalDevice& GetPhysicalDevice() { return m_PhysicalDevice; }



		/*const VulkanQueue& GetQueueByPresent(uint32_t queueIndex) const
		{
			for (uint32_t queueFamilyIndex = 0U; queueFamilyIndex < m_Queues.size(); ++queueFamilyIndex)
			{
				VulkanQueue const& firstQueue = m_Queues[queueFamilyIndex][0];

				uint32_t queue_count = firstQueue.GetProperties().queueCount;

				if (firstQueue.SupportPresent() && queueIndex < queue_count)
				{
					return m_Queues[queueFamilyIndex][queueIndex];
				}
			}

			KRONOS_CORE_ASSERT(false, "Queue not found!");
		}*/

    private:
    	const VulkanPhysicalDevice &m_PhysicalDevice;
        VkDevice m_Device;
		std::vector<std::vector<VulkanQueue>> m_Queues;
    };
}