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
        VulkanDevice(VulkanPhysicalDevice& physicalDevice, VkSurfaceKHR surface, const std::unordered_map<const char *, bool>& requestedExtensions = {})
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

			uint32_t availableExtensionsCount;
			vkEnumerateDeviceExtensionProperties(m_PhysicalDevice.GetHandle(), nullptr, &availableExtensionsCount, nullptr);
			std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
			vkEnumerateDeviceExtensionProperties(m_PhysicalDevice.GetHandle(), nullptr, &availableExtensionsCount, availableExtensions.data());

			std::vector<const char*> enabledExtensions = {};
			for (const std::pair<const char*, bool>& requestedExtension : requestedExtensions)
			{
				if (std::find_if(availableExtensions.begin(), availableExtensions.end(), [requestedExtension](auto& device_extension) {
					return std::strcmp(device_extension.extensionName, requestedExtension.first) == 0;
					}) != availableExtensions.end())
				{
					Log::Trace("Enabling extension: '{0}'"); // TODO: Log name
					enabledExtensions.push_back(requestedExtension.first);
				}
				else
				{
					if (!requestedExtension.second)
					{
						Log::Trace("Failed to enable optional extension, the extension was not available: '{0}'");// TODO: Log name
					}
					else
					{
						KRONOS_CORE_ASSERT(false, "Failed to enable required extension, the extension was not available: '{0}'");// TODO: Log name
					}
				}
			}

			VkDeviceCreateInfo deviceCreateInfo{};
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
			deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
			deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
			deviceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();

			// TODO: When to enable?
			const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
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