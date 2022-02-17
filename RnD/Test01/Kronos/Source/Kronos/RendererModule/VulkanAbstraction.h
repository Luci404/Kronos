#pragma once

#include "Kronos/Core/Assert.h"

#define VK_USE_PLATFORM_WIN32_KHR 
#include <vulkan/vulkan.h>

#include <array>
#include <string>
#include <vector>
#include <set>

namespace Kronos
{
	typedef uint32_t VulkanQueueFamilyIndex;

	struct VulkanConfiguration
	{
		std::string ApplicationName = "Kronos Application";
		bool Debug = true;
		bool Headless = false;
	};

	struct VulkanPhysicalDeviceSpecifications
	{


	public:
		VulkanPhysicalDeviceSpecifications(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface = VK_NULL_HANDLE) {}
	};

	// ptr vs reference
	/*
	SHORT: Some coding convention (like Google's) prescribe that one should always use pointers, or const references, because references have a bit of unclear-syntax: they have reference behaviour but value syntax.

	Besides allowing NULL values and dealing with raw arrays, it seems the choice comes down to personal preference.
	I've accepted the answer below that references Google's C++ Style Guide(https://google.github.io/styleguide/cppguide.html#Reference_Arguments),
	as they present the view that "References can be confusing, as they have value syntax but pointer semantics.".

	Due to the additional work required to sanitise pointer arguments that should not be NULL (e.g. add_one(0)
	will call the pointer version and break during runtime), it makes sense from a maintainability perspective to
	use references where an object MUST be present, though it is a shame to lose the syntactic clarity.
	*/
	/*class VulkanPhysicalDevice
	{
		friend class VulkanInstance;

	public:
		VkPhysicalDevice GetVkPhysicalDevice() const { return m_PhysicalDevice; }

		uint32_t GetGraphicsQueueFamilyIndex() const { return m_GraphicsQueueFamilyIndex; }
		uint32_t GetComputeQueueFamilyIndex() const { return m_ComputeQueueFamilyIndex; }
		uint32_t GetTransferQueueFamilyIndex() const { return m_TransferQueueFamilyIndex; }

	private:
		VulkanPhysicalDevice(VkPhysicalDevice physicalDevice)
			: m_PhysicalDevice(physicalDevice)
		{
			vkGetPhysicalDeviceProperties(m_PhysicalDevice, &m_Properties);
			vkGetPhysicalDeviceFeatures(m_PhysicalDevice, &m_Features);
			vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &m_MemoryProperties);

			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevice, &queueFamilyCount, nullptr);
			KRONOS_CORE_ASSERT(queueFamilyCount > 0, "Physical device has no queue families!");
			m_QueueFamilyProperties.resize(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, m_QueueFamilyProperties.data());

			uint32_t extensionPropertyCount = 0;
			vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionPropertyCount, nullptr);
			if (extensionPropertyCount > 0)
			{
				m_ExtensionProperties.resize(extensionPropertyCount);
				vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionPropertyCount, &m_ExtensionProperties.front());
			}
		}

	private:
		VkPhysicalDevice m_PhysicalDevice;

		uint32_t m_GraphicsQueueFamilyIndex;
		uint32_t m_ComputeQueueFamilyIndex;
		uint32_t m_TransferQueueFamilyIndex;
		
		VkPhysicalDeviceProperties m_Properties;
		VkPhysicalDeviceFeatures m_Features;
		VkPhysicalDeviceMemoryProperties m_MemoryProperties;
		std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
		std::vector<VkExtensionProperties> m_ExtensionProperties;

		VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> m_SurfaceFormats;
		std::vector<VkPresentModeKHR> m_SurfacePresentModes;
	};

	class VulkanInstance
	{
	public:
		VulkanInstance(VulkanConfiguration configuration)
			: m_VulkanConfiguration(configuration)
		{
			CreateInstance();
			GetPhysicalDevices();
		}

		VkInstance GetVkInstance() const { return m_Instance; }

		VulkanPhysicalDevice* GetFirstPhysicalDevice() { return &m_PhysicalDevices.front(); }

	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
		{
			std::cout << "Validation layer: " << pCallbackData->pMessage << std::endl;

			return VK_FALSE;
		}

		void CreateInstance()
		{
			VkApplicationInfo applicationInfo{};
			applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			applicationInfo.pNext = nullptr;
			applicationInfo.pApplicationName = m_VulkanConfiguration.ApplicationName.c_str();
			applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			applicationInfo.pEngineName = "Kronos Engine";
			applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			applicationInfo.apiVersion = VK_API_VERSION_1_0;

			std::vector<const char*> enabledInstanceLayers = {};
			if (m_VulkanConfiguration.Debug) enabledInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");

			std::vector<const char*> enabledInstanceExtensions = { "VK_KHR_surface", "VK_KHR_win32_surface" };
			if (m_VulkanConfiguration.Debug) enabledInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{};
			debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugUtilsMessengerCreateInfo.pfnUserCallback = &DebugUtilsMessengerCallback;

			VkInstanceCreateInfo instanceCreateInfo{};
			instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceCreateInfo.pNext = m_VulkanConfiguration.Debug ? (VkDebugUtilsMessengerCreateInfoEXT*)&debugUtilsMessengerCreateInfo : nullptr;
			instanceCreateInfo.flags = 0;
			instanceCreateInfo.pApplicationInfo = &applicationInfo;
			instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledInstanceLayers.size());
			instanceCreateInfo.ppEnabledLayerNames = enabledInstanceLayers.data();
			instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledInstanceExtensions.size());
			instanceCreateInfo.ppEnabledExtensionNames = enabledInstanceExtensions.data();
			KRONOS_CORE_ASSERT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) == VK_SUCCESS, "Failed to create instance!");

			if (m_VulkanConfiguration.Debug)
			{
				auto vkCreateDebugUtilsMessengerEXTProcAddr = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
				KRONOS_CORE_ASSERT(vkCreateDebugUtilsMessengerEXTProcAddr != nullptr, "Failed to get 'vkCreateDebugUtilsMessengerEXT' proc address!");
				KRONOS_CORE_ASSERT(vkCreateDebugUtilsMessengerEXTProcAddr(m_Instance, &debugUtilsMessengerCreateInfo, nullptr, &m_DebugMessenger) == VK_SUCCESS, "Failed to set up debug messenger!");
			}
		}

		void GetPhysicalDevices()
		{
			uint32_t physicalDeviceCount = 0;
			vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr);
			KRONOS_CORE_ASSERT(physicalDeviceCount > 0, "Failed to find physical device with Vulkan support!");

			std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
			vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, physicalDevices.data());

			for (const auto& physicalDevice : physicalDevices) {
				m_PhysicalDevices.push_back(VulkanPhysicalDevice(physicalDevice));
			}
		}

	private:
		VulkanConfiguration m_VulkanConfiguration;

		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		
		std::vector<VulkanPhysicalDevice> m_PhysicalDevices;
	};

	class VulkanSurface
	{
	public:
		VulkanSurface(VulkanInstance* instance, Window* window)
		{
			VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
			surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			surfaceCreateInfo.hwnd = window->GetHWND_TMP();
			surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
			KRONOS_CORE_ASSERT(vkCreateWin32SurfaceKHR(instance->GetVkInstance(), &surfaceCreateInfo, nullptr, &m_Surface) == VK_SUCCESS, "Failed to create window surface!");
		}

	private:
		VkSurfaceKHR m_Surface;
	};

	class VulkanDevice
	{
	public:
		VulkanDevice(VulkanInstance* instance, VulkanPhysicalDevice* physicalDevice)
			: m_Instance(instance), m_PhysicalDevice(physicalDevice)
		{
			std::set<uint32_t> uniqueQueueFamilies = {
				m_PhysicalDevice->GetGraphicsQueueFamilyIndex(),
				m_PhysicalDevice->GetComputeQueueFamilyIndex(),
				m_PhysicalDevice->GetTransferQueueFamilyIndex()
			};

			float defaultQueuePriority = 1.0f;
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			for (uint32_t queueFamily : uniqueQueueFamilies) {
				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamily;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &defaultQueuePriority;
				queueCreateInfos.push_back(queueCreateInfo);
			}

			const std::vector<const char*> enabledLayerNames = { "VK_LAYER_KHRONOS_validation" };

			const std::vector<const char*> enabledExtensionNames = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

			VkPhysicalDeviceFeatures enabledFeatures{};
			enabledFeatures.samplerAnisotropy = VK_TRUE;

			VkDeviceCreateInfo deviceCreateInfo{};
			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			deviceCreateInfo.pNext = nullptr;
			deviceCreateInfo.flags = 0;
			deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());;
			deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();;
			deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayerNames.size());
			deviceCreateInfo.ppEnabledLayerNames = enabledLayerNames.data();
			deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensionNames.size());;
			deviceCreateInfo.ppEnabledExtensionNames = enabledExtensionNames.data();
			deviceCreateInfo.pEnabledFeatures = &enabledFeatures;

			vkCreateDevice(m_PhysicalDevice->GetVkPhysicalDevice(), &deviceCreateInfo, nullptr, &m_LogicalDevice);
		}

		VulkanPhysicalDevice* GetPhysicalDevice() const { return m_PhysicalDevice; }

	private:
		VkDevice m_LogicalDevice;
	
		VulkanInstance* m_Instance;
		VulkanPhysicalDevice* m_PhysicalDevice;
	};*/

	/*class VulkanSwapchain
	{
	public:
		VulkanSwapchain(VulkanDevice* device, VulkanSurface* surface)
			: m_Instance(instance), m_Surface(surface)
		{
			VkSwapchainCreateInfoKHR swapchainCreateInfo{};
			swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchainCreateInfo.pNext = nullptr;
			swapchainCreateInfo.flags = 0;
			swapchainCreateInfo.surface = surface->GetVkSurfaceKHR();
			swapchainCreateInfo.minImageCount = ;
			swapchainCreateInfo.imageFormat =;
			swapchainCreateInfo.imageColorSpace =;
			swapchainCreateInfo.imageExtent =;
			swapchainCreateInfo.imageArrayLayers =;
			swapchainCreateInfo.imageUsage =;
			swapchainCreateInfo.imageSharingMode =;
			swapchainCreateInfo.queueFamilyIndexCount =;
			swapchainCreateInfo.pQueueFamilyIndices = ;
			swapchainCreateInfo.preTransform =;
			swapchainCreateInfo.compositeAlpha =;
			swapchainCreateInfo.presentMode =;
			swapchainCreateInfo.clipped =;
			swapchainCreateInfo.oldSwapchain =;
		}
	};*/
}