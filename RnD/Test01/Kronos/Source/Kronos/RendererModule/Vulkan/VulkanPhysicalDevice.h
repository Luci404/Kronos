#pragma once

#include "Kronos/Core/Assert.h"

#include "Kronos/RendererModule/Vulkan/VulkanInstance.h"

#define VK_USE_PLATFORM_WIN32_KHR 
#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace Kronos
{
    class VulkanPhysicalDevice
    {
    public:
        VulkanPhysicalDevice(VulkanInstance& instance, VkPhysicalDevice physicalDevice)
            : m_Instance(instance), m_PhysicalDevice(physicalDevice)
        {
	        vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);
	        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);
	        vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemoryProperties);

            uint32_t queueFamilyPropertiesCount = 0;
	        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyPropertiesCount, nullptr);
	        m_QueueFamilyProperties = std::vector<VkQueueFamilyProperties>(queueFamilyPropertiesCount);
	        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyPropertiesCount, m_QueueFamilyProperties.data());
        }

        VulkanPhysicalDevice(const VulkanPhysicalDevice&) = delete;
        VulkanPhysicalDevice(VulkanPhysicalDevice&&) = delete;
        VulkanPhysicalDevice&operator=(const VulkanPhysicalDevice&) = delete;
        VulkanPhysicalDevice&operator=(VulkanPhysicalDevice&&) = delete;

        VkPhysicalDevice GetHandle() const { return m_PhysicalDevice; }

        bool SupportsPresent(VkSurfaceKHR surface, uint32_t queueFamilyIndex) const
        {
	        VkBool32 supportsPresent = false;

	        if (surface != VK_NULL_HANDLE)
	        {
		        vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevice, queueFamilyIndex, surface, &supportsPresent);
	        }

	        return (bool)supportsPresent;
        }

        VulkanInstance& GetInstance() const { return m_Instance; }

        const VkPhysicalDeviceFeatures& GetFeatures() const { return m_Features; }
        const VkPhysicalDeviceProperties& GetProperties() const { return m_Properties; }
        const VkPhysicalDeviceMemoryProperties GetMemoryProperties() const { return m_MemoryProperties; }
        const std::vector<VkQueueFamilyProperties>& GetQueueFamilyProperties() const { return m_QueueFamilyProperties; }
    
    private:
        VulkanInstance& m_Instance;
        VkPhysicalDevice m_PhysicalDevice;

    	VkPhysicalDeviceFeatures m_Features{};
    	VkPhysicalDeviceProperties m_Properties;
    	VkPhysicalDeviceMemoryProperties m_MemoryProperties;
	    std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
    };
}