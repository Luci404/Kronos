#include "Kronos/RendererModule/Vulkan/VulkanQueue.h"
#include "Kronos/RendererModule/Vulkan/VulkanDevice.h"

namespace Kronos
{
	VulkanQueue::VulkanQueue(VulkanDevice& device, uint32_t familyIndex, VkQueueFamilyProperties properties, VkBool32 supportPresent, uint32_t index)
		: m_Device(device), m_Handle(VK_NULL_HANDLE), m_FamilyIndex(familyIndex), m_Index(index), m_SupportPresent(supportPresent), m_Properties(properties)
	{
		vkGetDeviceQueue(m_Device.GetHandle(), m_FamilyIndex, m_Index, &m_Handle);
	}

    VulkanQueue::VulkanQueue(VulkanQueue&& other)
        : m_Device(other.m_Device), m_Handle(other.m_Handle), m_FamilyIndex(other.m_FamilyIndex), m_Index(other.m_Index), m_SupportPresent(other.m_SupportPresent), m_Properties(other.m_Properties)
    {
	    other.m_Handle = VK_NULL_HANDLE;
	    other.m_FamilyIndex = {};
	    other.m_Properties = {};
	    other.m_SupportPresent = VK_FALSE;
	    other.m_Index = 0;
    }

    VkResult VulkanQueue::Submit(const std::vector<VkSubmitInfo>& submitInfos, VkFence fence) const
	{
		return vkQueueSubmit(m_Handle, static_cast<uint32_t>(submitInfos.size()), submitInfos.data(), fence);
	}

	VkResult VulkanQueue::Present(const VkPresentInfoKHR presentInfo) const
	{
		KRONOS_CORE_ASSERT(m_SupportPresent, "Failed to present, the queue does not have present support!");
		return vkQueuePresentKHR(m_Handle, &presentInfo);
	}
}