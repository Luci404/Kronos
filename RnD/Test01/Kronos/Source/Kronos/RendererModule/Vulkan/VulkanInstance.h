#pragma once

#include "Kronos/LoggingModule/LoggingModule.h"
#include "Kronos/Core/Assert.h"

#define VK_USE_PLATFORM_WIN32_KHR 
#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <set>
#include <unordered_map>

namespace Kronos
{
	class VulkanInstance
	{
	public:
		VulkanInstance(const std::string& applicationName,
			const std::unordered_map<const char*, bool>& requestedExtensions = {},
			const std::unordered_map<const char*, bool>& requestedLayers = {}
		/*TODO: bool headless = false */)
		{

			bool debug = true;

			VkApplicationInfo applicationInfo{};
			applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			applicationInfo.pNext = nullptr;
			applicationInfo.pApplicationName = "Kronos Application";
			applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			applicationInfo.pEngineName = "Kronos Engine";
			applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			applicationInfo.apiVersion = VK_API_VERSION_1_0;

			uint32_t availableExtensionsCount;
			vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr);
			std::vector<VkExtensionProperties> availableExtensions(availableExtensionsCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableExtensions.data());

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
						Log::Trace("Failed to enable optional extenstion, the extention was not available: '{0}'"); // TODO: Log name
					}
					else
					{
						KRONOS_CORE_ASSERT(false, "Failed to enable required extenstion, the extention was not available: '{0}'"); // TODO: Log name
					}
				}
			}

			uint32_t availableLayerCount;
			vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
			std::vector<VkLayerProperties> availableLayers(availableLayerCount);
			vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

			std::vector<const char*> enabledLayers = {};
			for (const std::pair<const char*, bool>& requestedLayer : requestedLayers)
			{
				if (std::find_if(availableLayers.begin(), availableLayers.end(), [requestedLayer](auto& device_extension) {
					return std::strcmp(device_extension.layerName, requestedLayer.first) == 0;
					}) != availableLayers.end())
				{
					Log::Trace("Enabling layer: '{0}'"); // TODO: Log name
					enabledLayers.push_back(requestedLayer.first);
				}
				else
				{
					if (!requestedLayer.second)
					{
						Log::Trace("Failed to enable optional layer, the layer was not available: '{0}'");// TODO: Log name
					}
					else
					{
						KRONOS_CORE_ASSERT(false, "Failed to enable required layer, the layer was not available: '{0}'");// TODO: Log name
					}
				}
			}

			VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{};
			debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugUtilsMessengerCreateInfo.pfnUserCallback = &DebugUtilsMessengerCallback;

			VkInstanceCreateInfo instanceCreateInfo{};
			instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceCreateInfo.pNext = debug ? (VkDebugUtilsMessengerCreateInfoEXT*)&debugUtilsMessengerCreateInfo : nullptr;
			instanceCreateInfo.flags = 0;
			instanceCreateInfo.pApplicationInfo = &applicationInfo;
			instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
			instanceCreateInfo.ppEnabledLayerNames = enabledLayers.data();
			instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
			instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
			KRONOS_CORE_ASSERT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) == VK_SUCCESS, "Failed to create instance!");

			if (debug)
			{
				auto vkCreateDebugUtilsMessengerEXTProcAddr = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
				KRONOS_CORE_ASSERT(vkCreateDebugUtilsMessengerEXTProcAddr != nullptr, "Failed to get 'vkCreateDebugUtilsMessengerEXT' proc address!");
				KRONOS_CORE_ASSERT(vkCreateDebugUtilsMessengerEXTProcAddr(m_Instance, &debugUtilsMessengerCreateInfo, nullptr, &m_DebugMessenger) == VK_SUCCESS, "Failed to set up debug messenger!");
			}
		}

		VulkanInstance(const VulkanInstance&) = delete;
		VulkanInstance(VulkanInstance&&) = delete;
		VulkanInstance& operator=(const VulkanInstance&) = delete;
		VulkanInstance& operator=(VulkanInstance&&) = delete;

		VkInstance GetHandle() const { return m_Instance; };

	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
		{
			std::cout << "Validation layer: " << pCallbackData->pMessage << std::endl;
			return VK_FALSE;
		}

	private:
		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
	};
}