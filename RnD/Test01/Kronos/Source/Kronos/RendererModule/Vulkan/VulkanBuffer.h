#pragma once

#define VK_USE_PLATFORM_WIN32_KHR 
#include <vulkan/vulkan.h>

namespace Kronos
{
    class VulkanBuffer
    {
    public:
        VulkanBuffer() {}
        ~VulkanBuffer() {}
    };
}