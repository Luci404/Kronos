#pragma once

#include "Kronos/Core/Assert.h"

#include "Kronos/RendererModule/Vulkan/VulkanPhysicalDevice.h"

#define VK_USE_PLATFORM_WIN32_KHR 
#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace Kronos
{
    class VulkanDevice
    {
    public:
        VulkanDevice(VulkanPhysicalDevice& physicalDevice, VkSurfaceKHR surface, std::unordered_map<const char *, bool> requestedExtensions = {})
            : m_PhysicalDevice(physicalDevice)
        {
            uint32_t queueFamilyPropertiesCount = m_PhysicalDevice.GetQueueFamilyProperties().size();
            std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(queueFamilyPropertiesCount, { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO });

			const float defaultQueuePriorities = 0.5f;
			for (uint32_t queueFamilyIndex = 0U; queueFamilyIndex < queueFamilyPropertiesCount; ++queueFamilyIndex)
			{
				const VkQueueFamilyProperties& queueFamilyProperty = m_PhysicalDevice.GetQueueFamilyProperties()[queueFamilyIndex];
				VkDeviceQueueCreateInfo& queueCreateInfo = queueCreateInfos[queueFamilyIndex];
				queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
				queueCreateInfo.queueCount = queueFamilyProperty.queueCount;
				queueCreateInfo.pQueuePriorities = &defaultQueuePriorities;
			}

			VkPhysicalDeviceFeatures deviceFeatures{};
			deviceFeatures.samplerAnisotropy = VK_TRUE;

			const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
			const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

			VkDeviceCreateInfo deviceCreateInfo{};
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
			deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
			deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

			// TODO: When to enable?
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();

			if (vkCreateDevice(m_PhysicalDevice.GetHandle(), &deviceCreateInfo, nullptr, &m_Device) != VK_SUCCESS) {
				std::cout << "Failed to create logical device!" << std::endl;
				return;
			}
        }

       	VulkanDevice(const VulkanDevice &) = delete;
    	VulkanDevice(VulkanDevice &&) = delete;
	    VulkanDevice &operator=(const VulkanDevice &) = delete;
	    VulkanDevice &operator=(VulkanDevice &&) = delete;

        VkDevice GetHandle() const { return m_Device; }

    private:
    	const VulkanPhysicalDevice &m_PhysicalDevice;
        VkDevice m_Device;
    };
}