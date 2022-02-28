#include "Kronos/RendererModule/StaticMesh.h"

#define VK_USE_PLATFORM_WIN32_KHR
#include "vulkan/vulkan.h"

namespace Kronos
{
    StaticMesh::StaticMesh(const VulkanDevice& device, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
        : m_VertexBuffer(nullptr), m_IndexBuffer(nullptr)
    {
        /*VkDeviceSize size = sizeof(vertices[0]) * vertices.size();
        m_VertexBuffer = CreateScope<VulkanBuffer>(device, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(m_Device->GetHandle(), stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, m_Vertices.data(), (size_t)bufferSize);
		vkUnmapMemory(m_Device->GetHandle(), stagingBufferMemory);

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

		CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

		vkDestroyBuffer(m_Device->GetHandle(), stagingBuffer, nullptr);
		vkFreeMemory(m_Device->GetHandle(), stagingBufferMemory, nullptr);

        //*/

        /*
        VkBufferUsageFlags bufferUsageFlags
        VmaMemoryUsage memoryUsage
        VmaAllocationCreateFlags flags
        const std::vector<uint32_t>& queueFamilyIndices
        */

        /*VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
        VulkanBuffer stagingBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

        m_VertexBuffer = CreateScope<VulkanBuffer>(device, bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);*/

    }
}