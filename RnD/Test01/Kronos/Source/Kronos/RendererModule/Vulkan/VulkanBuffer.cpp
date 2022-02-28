#include "Kronos/RendererModule/Vulkan/VulkanBuffer.h"
#include "Kronos/RendererModule/Vulkan/VulkanDevice.h"
#include "Kronos/Core/Assert.h"

namespace Kronos
{
	VulkanBuffer::VulkanBuffer(VulkanDevice& device, VkDeviceSize size, VkBufferUsageFlags bufferUsageFlags, VmaMemoryUsage memoryUsage, VmaAllocationCreateFlags flags, const std::vector<uint32_t>& queueFamilyIndices)
		: m_Device(device), m_Buffer(VK_NULL_HANDLE), m_Allocation(VK_NULL_HANDLE), m_DeviceMemory(VK_NULL_HANDLE), m_Size(size), m_MappedData(nullptr), m_Mapped(false), m_PersistentlyMapped(false)
	{
/*#ifdef VK_USE_PLATFORM_METAL_EXT
		// Workaround for Mac (MoltenVK requires unmapping https://github.com/KhronosGroup/MoltenVK/issues/175)
		// Force cleares the flag VMA_ALLOCATION_CREATE_MAPPED_BIT
		flags &= ~VMA_ALLOCATION_CREATE_MAPPED_BIT;
#endif*/

		m_PersistentlyMapped = (flags & VMA_ALLOCATION_CREATE_MAPPED_BIT) != 0;

		VkBufferCreateInfo bufferCreateInfo{};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.usage = bufferUsageFlags;
		bufferCreateInfo.size = m_Size;
		if (queueFamilyIndices.size() >= 2)
		{
			bufferCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			bufferCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
			bufferCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
		}

		VmaAllocationCreateInfo allocationCreateInfo{};
		allocationCreateInfo.flags = flags;
		allocationCreateInfo.usage = memoryUsage;

		VmaAllocationInfo allocationInfo{};
		KRONOS_CORE_ASSERT(vmaCreateBuffer(m_Device.GetMemoryAllocator(), &bufferCreateInfo, &allocationCreateInfo, &m_Buffer, &m_Allocation, &allocationInfo) == VK_SUCCESS, "Cannot create Buffer");
		
		m_DeviceMemory = allocationInfo.deviceMemory;

		if (m_PersistentlyMapped)
		{
			m_MappedData = static_cast<uint8_t*>(allocationInfo.pMappedData);
		}
	}
		
	VulkanBuffer::~VulkanBuffer()
	{
		if (m_Buffer != VK_NULL_HANDLE && m_Allocation != VK_NULL_HANDLE)
		{
			Unmap();
			vmaDestroyBuffer(m_Device.GetMemoryAllocator(), m_Buffer, m_Allocation);
		}
	}

	uint8_t* VulkanBuffer::Map()
	{
		if (!m_Mapped && !m_MappedData)
		{
			vmaMapMemory(m_Device.GetMemoryAllocator(), m_Allocation, reinterpret_cast<void**>(&m_MappedData));
			m_Mapped = true;
		}
		return m_MappedData;
	}

	void VulkanBuffer::Unmap()
	{
		if (m_Mapped)
		{
			vmaUnmapMemory(m_Device.GetMemoryAllocator(), m_Allocation);
			m_MappedData = nullptr;
			m_Mapped = false;
		}
	}

	void VulkanBuffer::Flush() const
	{
		vmaFlushAllocation(m_Device.GetMemoryAllocator(), m_Allocation, 0, m_Size);
	}

	void VulkanBuffer::Update(const uint8_t* data, const uint32_t size, const uint32_t offset)
	{
		if (m_PersistentlyMapped)
		{
			std::copy(data, data + size, m_MappedData + offset);
			Flush();
		}
		else
		{
			Map();
			std::copy(data, data + size, m_MappedData + offset);
			Flush();
			Unmap();
		}
	}

}