#include "Kronos/RendererModule/Vulkan/VulkanCommandPool.h"
#include "Kronos/RendererModule/Vulkan/VulkanDevice.h"

namespace Kronos
{
    VulkanCommandPool::VulkanCommandPool(VulkanDevice& device, uint32_t queueFamilyIndex)
        : m_CommandPool(VK_NULL_HANDLE), m_Device(device)
    {
		VkCommandPoolCreateInfo commandPoolCreateInfo{};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;

		KRONOS_CORE_ASSERT(vkCreateCommandPool(m_Device.GetHandle(), &commandPoolCreateInfo, nullptr, &m_CommandPool) == VK_SUCCESS, "Failed to create command pool!");
    }

    VulkanCommandPool::~VulkanCommandPool()
    {
        vkDestroyCommandPool(m_Device.GetHandle(), m_CommandPool, nullptr);
    }
}