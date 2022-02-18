#include "Kronos/RendererModule/Vulkan/VulkanDevice.h"

#include "Kronos/RendererModule/Vulkan/VulkanQueue.h"

namespace Kronos
{
	VulkanDevice::VulkanDevice(VulkanPhysicalDevice& physicalDevice, VkSurfaceKHR surface, const std::unordered_map<const char*, bool>& requestedExtensions)
		: m_PhysicalDevice(physicalDevice)
	{
		uint32_t queueFamilyPropertiesCount = m_PhysicalDevice.GetQueueFamilyProperties().size();
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(queueFamilyPropertiesCount, { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO });
		std::vector<std::vector<float>> queuePriorities(queueFamilyPropertiesCount);

		for (uint32_t queueFamilyIndex = 0U; queueFamilyIndex < queueFamilyPropertiesCount; ++queueFamilyIndex)
		{
			const VkQueueFamilyProperties& queueFamilyProperty = m_PhysicalDevice.GetQueueFamilyProperties()[queueFamilyIndex];
			queuePriorities[queueFamilyIndex].resize(queueFamilyProperty.queueCount, 0.5f);

			VkDeviceQueueCreateInfo& queueCreateInfo = queueCreateInfos[queueFamilyIndex];
			queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
			queueCreateInfo.queueCount = queueFamilyProperty.queueCount;
			queueCreateInfo.pQueuePriorities = queuePriorities[queueFamilyIndex].data();
		}

		m_Queues.resize(queueFamilyPropertiesCount);
		for (uint32_t queueFamilyIndex = 0U; queueFamilyIndex < queueFamilyPropertiesCount; ++queueFamilyIndex)
		{
			const VkQueueFamilyProperties& queueFamilyProperty = m_PhysicalDevice.GetQueueFamilyProperties()[queueFamilyIndex];
			VkBool32 supportsPresent = m_PhysicalDevice.SupportsPresent(surface, queueFamilyIndex);

			for (uint32_t queueIndex = 0U; queueIndex < queueFamilyProperty.queueCount; ++queueIndex)
			{
				m_Queues[queueFamilyIndex].emplace_back(*this, queueFamilyIndex, queueFamilyProperty, supportsPresent, queueIndex);
			}
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

	const VulkanQueue& VulkanDevice::GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const
	{
		return m_Queues[queueFamilyIndex][queueIndex];
	}

	const VulkanQueue& VulkanDevice::GetQueueByFlags(VkQueueFlags requiredQueueFlags, uint32_t queueIndex) const
	{
		for (uint32_t queueFamilyIndex = 0U; queueFamilyIndex < m_Queues.size(); ++queueFamilyIndex)
		{
			VulkanQueue const& firstQueue = m_Queues[queueFamilyIndex][0];

			VkQueueFlags queueFlags = firstQueue.GetProperties().queueFlags;
			uint32_t queueCount = firstQueue.GetProperties().queueCount;

			if (((queueFlags & requiredQueueFlags) == requiredQueueFlags) && queueIndex < queueCount)
			{
				return m_Queues[queueFamilyIndex][queueIndex];
			}
		}

		KRONOS_CORE_ASSERT(false, "Queue not found!");
	}
}