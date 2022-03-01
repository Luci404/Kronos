#pragma once

#include "Kronos/Core/Assert.h"
#include "Kronos/Core/Memory.h"

#include "Kronos/RendererModule/Vulkan/VulkanCommon.h"
#include "Kronos/RendererModule/Vulkan/VulkanCommandPool.h"

#include <vk_mem_alloc.h>

#include <string>
#include <vector>
#include <unordered_map>

namespace Kronos
{
	class VulkanQueue;
	class VulkanBuffer;
	class VulkanPhysicalDevice;

    class VulkanDevice
    {
    public:
        VulkanDevice(VulkanPhysicalDevice& physicalDevice, VkSurfaceKHR surface, const std::unordered_map<const char *, bool>& requestedExtensions = {});
		~VulkanDevice();

       	VulkanDevice(const VulkanDevice &) = delete;
    	VulkanDevice(VulkanDevice &&) = delete;
	    VulkanDevice &operator=(const VulkanDevice &) = delete;
	    VulkanDevice &operator=(VulkanDevice &&) = delete;

        VkDevice GetHandle() const { return m_Device; }

		const VulkanQueue& GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const;

		const VulkanQueue& GetQueueByFlags(VkQueueFlags requiredQueueFlags, uint32_t queueIndex) const;

		const VulkanPhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }

		VmaAllocator GetMemoryAllocator() const { return m_MemoryAllocator; }
		
		bool IsExtensionSupported(const std::string& requested_extension) const;

		bool IsExtensionEnabled(const char* extension) const;

		VkCommandBuffer CreateCommandBuffer(VkCommandBufferLevel comandBufferLevel, bool begin = false) const;

		void SubmitCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, bool free = true, VkSemaphore signalSemaphore = VK_NULL_HANDLE) const;

		void CopyBuffer(VulkanBuffer& source, VulkanBuffer& destination, VkQueue queue, VkBufferCopy* copyRegion = nullptr);

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
		std::vector<VkExtensionProperties> m_DeviceExtensions;
		std::vector<const char*> m_EnabledExtensions{};
		VmaAllocator m_MemoryAllocator{ VK_NULL_HANDLE };
		Scope<VulkanCommandPool> m_CommandPool;
	};
}