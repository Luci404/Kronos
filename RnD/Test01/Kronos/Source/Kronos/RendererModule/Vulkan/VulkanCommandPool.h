#pragma once

#include "Kronos/RendererModule/Vulkan/VulkanCommon.h"

namespace Kronos
{
    class VulkanDevice;

    class VulkanCommandPool
    {
    public:
        VulkanCommandPool(VulkanDevice& device, uint32_t queueFamilyIndex);
        ~VulkanCommandPool();

        VkCommandPool GetHandle() const { return m_CommandPool; }
        VulkanDevice& GetDevice() const { return m_Device; }

    private:
        VkCommandPool m_CommandPool;
        VulkanDevice& m_Device;
    };
}