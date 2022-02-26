#include "Kronos/RendererModule/Vulkan/VulkanDevice.h"
#include "Kronos/RendererModule/Vulkan/VulkanQueue.h"

#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace Kronos
{
	VulkanDevice::VulkanDevice(VulkanPhysicalDevice& physicalDevice, VkSurfaceKHR surface, const std::unordered_map<const char*, bool>& requestedExtensions)
		: m_PhysicalDevice(physicalDevice)
	{
		uint32_t deviceExtensionCount;
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice.GetHandle(), nullptr, &deviceExtensionCount, nullptr);
		m_DeviceExtensions = std::vector<VkExtensionProperties>(deviceExtensionCount);
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice.GetHandle(), nullptr, &deviceExtensionCount, m_DeviceExtensions.data());

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

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		uint32_t availableExtensionsCount;
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice.GetHandle(), nullptr, &availableExtensionsCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
		vkEnumerateDeviceExtensionProperties(m_PhysicalDevice.GetHandle(), nullptr, &availableExtensionsCount, availableExtensions.data());

		for (const std::pair<const char*, bool>& requestedExtension : requestedExtensions)
		{
			if (std::find_if(availableExtensions.begin(), availableExtensions.end(), [requestedExtension](auto& device_extension) {
				return std::strcmp(device_extension.extensionName, requestedExtension.first) == 0;
				}) != availableExtensions.end())
			{
				Log::Trace("Enabling extension: '{0}'"); // TODO: Log name
				m_EnabledExtensions.push_back(requestedExtension.first);
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

		if (IsExtensionSupported("VK_KHR_get_memory_requirements2") && IsExtensionSupported("VK_KHR_dedicated_allocation"))
		{
			m_EnabledExtensions.push_back("VK_KHR_get_memory_requirements2");
			m_EnabledExtensions.push_back("VK_KHR_dedicated_allocation");

			Log::Trace("Dedicated Allocation enabled");
		}

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_EnabledExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = m_EnabledExtensions.data();

		// TODO: When to enable?
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();

		if (vkCreateDevice(m_PhysicalDevice.GetHandle(), &deviceCreateInfo, nullptr, &m_Device) != VK_SUCCESS)
		{
			std::cout << "Failed to create logical device!" << std::endl;
			return;
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

		// NEW
		VmaVulkanFunctions vma_vulkan_func{};
		vma_vulkan_func.vkAllocateMemory = vkAllocateMemory;
		vma_vulkan_func.vkBindBufferMemory = vkBindBufferMemory;
		vma_vulkan_func.vkBindImageMemory = vkBindImageMemory;
		vma_vulkan_func.vkCreateBuffer = vkCreateBuffer;
		vma_vulkan_func.vkCreateImage = vkCreateImage;
		vma_vulkan_func.vkDestroyBuffer = vkDestroyBuffer;
		vma_vulkan_func.vkDestroyImage = vkDestroyImage;
		vma_vulkan_func.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
		vma_vulkan_func.vkFreeMemory = vkFreeMemory;
		vma_vulkan_func.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
		vma_vulkan_func.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
		vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
		vma_vulkan_func.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
		vma_vulkan_func.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
		vma_vulkan_func.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
		vma_vulkan_func.vkMapMemory = vkMapMemory;
		vma_vulkan_func.vkUnmapMemory = vkUnmapMemory;
		vma_vulkan_func.vkCmdCopyBuffer = vkCmdCopyBuffer;

		VmaAllocatorCreateInfo allocator_info{};
		allocator_info.physicalDevice = m_PhysicalDevice.GetHandle();
		allocator_info.device = m_Device;
		allocator_info.instance = m_PhysicalDevice.GetInstance().GetHandle();

		// Check extensions to enable Vma Dedicated Allocation
		if (IsExtensionSupported("VK_KHR_get_memory_requirements2") && IsExtensionSupported("VK_KHR_dedicated_allocation"))
		{
			allocator_info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
			vma_vulkan_func.vkGetBufferMemoryRequirements2KHR = PFN_vkGetBufferMemoryRequirements2KHR(vkGetDeviceProcAddr(m_Device, "vkGetBufferMemoryRequirements2KHR"));
			vma_vulkan_func.vkGetImageMemoryRequirements2KHR = PFN_vkGetImageMemoryRequirements2KHR(vkGetDeviceProcAddr(m_Device, "vkGetImageMemoryRequirements2KHR"));
		}

		if (IsExtensionSupported(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) && IsExtensionEnabled(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
		{
			allocator_info.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		}

		allocator_info.pVulkanFunctions = &vma_vulkan_func;

		KRONOS_CORE_ASSERT(vmaCreateAllocator(&allocator_info, &m_MemoryAllocator) == VK_SUCCESS, "Cannot create allocator");
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

	bool VulkanDevice::IsExtensionSupported(const std::string& requestedExtension)
	{
		return std::find_if(m_DeviceExtensions.begin(), m_DeviceExtensions.end(),
			[requestedExtension](auto& deviceExtension) {
				return std::strcmp(deviceExtension.extensionName, requestedExtension.c_str()) == 0;
			}) != m_DeviceExtensions.end();
	}

	bool VulkanDevice::IsExtensionEnabled(const char* extension)
	{
		return std::find_if(m_EnabledExtensions.begin(), m_EnabledExtensions.end(), [extension](const char* enabled_extension) { return strcmp(extension, enabled_extension) == 0; }) != m_EnabledExtensions.end();
	}
}