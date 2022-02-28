#pragma once

#define VK_USE_PLATFORM_WIN32_KHR 
#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

#include <vector>

namespace Kronos
{
    class VulkanDevice;

    class VulkanBuffer
    {
    public:
        VulkanBuffer(VulkanDevice& device, VkDeviceSize size, VkBufferUsageFlags bufferUsageFlags, VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags flags = VMA_ALLOCATION_CREATE_MAPPED_BIT, const std::vector<uint32_t>& queueFamilyIndices = {});
        ~VulkanBuffer();

        VkBuffer GetHandle() const { return m_Buffer; }
        VkDeviceSize GetSize() const { return m_Size; }
        
        uint8_t* Map();
        void Unmap();
        void Flush() const;
        void Update(const uint8_t* data, const uint32_t size, const uint32_t offset);

    private:
        VkBuffer m_Buffer;
        VulkanDevice& m_Device;
        VmaAllocation m_Allocation;
        VkDeviceMemory m_DeviceMemory;
        VkDeviceSize m_Size;
        uint8_t* m_MappedData;
        bool m_Mapped;
        bool m_PersistentlyMapped;
    };
}