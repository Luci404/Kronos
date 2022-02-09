#pragma once

#include <vulkan/vulkan.h>

namespace Kronos
{
    typedef uint32_t VulkanQueueFamilyIndex;

    struct VulkanConfiguration
    {
        bool Debug = true;
        bool Headless = false;
    };

    struct VulkanPhysicalDeviceSpecifications
    {
        VkPhysicalDeviceProperties Properties;
        VkPhysicalDeviceFeatures Features;
        VkPhysicalDeviceMemoryProperties DeviceMemoryProperties;
        std::vector<VkQueueFamilyProperties> QueueFamilyProperties;
        std::vector<const std::string> SupportedExtensions;

    public:
        VulkanPhysicalDeviceSpecifications(VkPhysicalDevice physicalDevice)
        {
        }
    };

    struct VulkanPhysicalDeviceSurfaceSpecifications
    {
        VkSurfaceCapabilitiesKHR SurfaceCapabilities;
        std::vector<VkSurfaceFormatKHR> SurfaceFormats;
        std::vector<VkPresentModeKHR> SurfacePresentModes;

    public:
        VulkanPhysicalDeviceSpecifications(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
        {
        }
    };

    class VulkanDevice 
    {
    public:
        VulkanDevice(VulkanConfiguration vulkanConfiguration)
            : m_VulkanConfiguration(vulkanConfiguration)
        {
            KRONOS_CORE_ASSERT(m_VulkanConfiguration.Headless, "Devices created with the intend of non-headless rendering should be using a constructor with a surface parameter!");
        }

        VulkanDevice(VulkanConfiguration vulkanConfiguration, VkSurfaceKHR surface)
            : m_VulkanConfiguration(vulkanConfiguration), m_Surface(surface)
        {
            KRONOS_CORE_ASSERT(!m_VulkanConfiguration.Headless, "Devices created with the intend of headless rendering should not be using a constructor with a surface parameter!");
        }

    private:
        void ChoosePhysicalDevice()
        {
            auto evaluatePhysicalDevice = [&](const VkPhysicalDevice& physicalDevice) -> uint32_t {
                VulkanPhysicalDeviceSpecifications physicalDeviceSpecifications = VulkanPhysicalDeviceSpecifications(physicalDevice);
                // Make sure everything fits the requirements...

                if (!m_VulkanConfiguration.Headless)
                {
                    VulkanPhysicalDeviceSurfaceSpecifications physicalDeviceSurfaceSpecifications = VulkanPhysicalDeviceSurfaceSpecifications(physicalDevice, surface);
                    // Make sure everything fits the requirements, what surface specs concerns... prioritize the the best present mode etc.
                }
            };
        }

    private:
        VulkanConfiguration m_VulkanConfiguration;
        VkSurfaceKHR m_Surface;
    };

    class VulkanSurface {};
    class VulkanSwapchain {};

    class VulkanDevice
    {
    public:
        VulkanDevice(VulkanConfiguration vulkanConfiguration)
            : m_VulkanConfiguration(vulkanConfiguration)
        {
            CreateInstance();
            ChoosePhysicalDevice();
            CreateLogicalDevice();
        };


        void CreateBuffer() const {};

        VulkanQueueFamilyIndex GetPresentQueueFamilyIndex() const {};
        VulkanQueueFamilyIndex GetGraphicsQueueFamilyIndex() const {};
    
    private:
        VKAPI_ATTR VkBool32 VKAPI_CALL DebugUtilsMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
        {
            std::cout << "Validation layer: " << pCallbackData->pMessage << std::endl;

            return VK_FALSE;
        }

        void CreateInstance()
        {
            VkApplicationInfo applicationInfo{};
            applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            applicationInfo.pNext = nullptr;
            applicationInfo.pApplicationName = "Kronos Application";
            applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            applicationInfo.pEngineName = "Kronos Engine";
            applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
            applicationInfo.apiVersion = VK_API_VERSION_1_0;

            std::vector<const char*> enabledInstanceLayers = {};
            if (m_VulkanConfiguration.EnableValidationLayers) enabledInstanceLayers.push_back("VK_LAYER_KHRONOS_validation");

            std::vector<const char*> enabledInstanceExtensions = { "VK_KHR_surface", "VK_KHR_win32_surface" };
            if (m_VulkanConfiguration.EnableValidationLayers) enabledInstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

            VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfo{};
            debugUtilsMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugUtilsMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debugUtilsMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debugUtilsMessengerCreateInfo.pfnUserCallback = DebugUtilsMessengerCallback;

            VkInstanceCreateInfo instanceCreateInfo{};
            instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCreateInfo.pNext = m_VulkanConfiguration.EnableValidationLayers ? (VkDebugUtilsMessengerCreateInfoEXT*)&debugUtilsMessengerCreateInfo : nullptr;
            instanceCreateInfo.flags = 0;
            instanceCreateInfo.pApplicationInfo = &applicationInfo;
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledInstanceLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = enabledInstanceLayers.data();
            instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledInstanceExtensions.size());
            instanceCreateInfo.ppEnabledExtensionNames = enabledInstanceExtensions.data();
            KRONOS_CORE_ASSERT(vkCreateInstance(&createInfo, nullptr, &m_Instance) == VK_SUCCESS, "Failed to create instance!");

            if (m_VulkanConfiguration.EnableValidationLayers)
            {
                auto vkCreateDebugUtilsMessengerEXTProcAddr = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
                KRONOS_CORE_ASSERT(vkCreateDebugUtilsMessengerEXTProcAddr != nullptr, "Failed to get 'vkCreateDebugUtilsMessengerEXT' proc address!");
                KRONOS_CORE_ASSERT(vkCreateDebugUtilsMessengerEXTProcAddr(m_Instance, &createInfo, nullptr, &m_DebugMessenger) == VK_SUCCESS, "Failed to set up debug messenger!");
            }
        }
    
        void ChoosePhysicalDevice()
        {
            uint32_t physicalDeviceCount = 0;
            vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, nullptr);
            KRONOS_CORE_ASSERT(physicalDeviceCount > 0, "Failed to find physical device with Vulkan support!");

            std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
            vkEnumeratePhysicalDevices(m_Instance, &physicalDeviceCount, physicalDevices.data());

            auto evaluatePhysicalDevice = [](const VkPhysicalDevice& physicalDevice) -> uint32_t {
                uint32_t deviceExtensionCount = 0;
                vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);
                std::vector<VkExtensionProperties> deviceExtensions(deviceExtensionCount);
                vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, deviceExtensions.data());
                
                bool deviceSupportsRequiredDeviceExtensions = true;
                std::set<std::string> requiredDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
                for (const auto& requiredDeviceExtension : requiredDeviceExtensions) {
                    bool deviceExtensionsContainsrequiredDeviceExtension = std::count(deviceExtensions.begin(), deviceExtensions.end(), requiredDeviceExtension);
                    deviceSupportsRequiredDeviceExtensions = deviceSupportsRequiredDeviceExtensions ? deviceExtensionsContainsrequiredDeviceExtension : false;
                }

                bool deviceSupportsRequiredSwapchainFeatures = true;
                if (deviceSupportsRequiredDeviceExtensions)
                {
                    VkSurfaceCapabilitiesKHR capabilities;
                    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_Surface, &capabilities);

                    std::vector<VkPresentModeKHR> presentModes;

                    std::vector<VkSurfaceFormatKHR> formats;
                    
                }

                bool deviceSupportsRequiredDeviceFeatures = true;
                VkPhysicalDeviceFeatures supportedDeviceFeatures;
                vkGetPhysicalDeviceFeatures(physicalDevice, &supportedDeviceFeatures);
                deviceSupportsRequiredDeviceFeatures = deviceSupportsRequiredDeviceFeatures ? supportedDeviceFeatures.samplerAnisotropy : false;

                bool deviceMeetsMinimumRequirements = deviceSupportsRequiredDeviceExtensions && deviceSupportsRequiredSwapchainFeatures && deviceSupportsRequiredDeviceFeatures;
                if (!deviceMeetsMinimumRequirements) { return 0; }
            };

            uint32_t chosenPhysicalDeviceScore = 0;
            VkPhysicalDevice chosenPhysicalDevice = VK_NULL_HANDLE;
            for (const auto& physicalDevice : physicalDevices) {
                uint32_t physicalDeviceScore = evaluatePhysicalDevice(physicalDevice);
                if (physicalDeviceScore >= 1 && physicalDeviceScore > chosenPhysicalDeviceScore)
                {
                    chosenPhysicalDevice = physicalDevice;
                    chosenPhysicalDeviceScore = physicalDeviceScore;
                }
            }
            m_PhysicalDevice = chosenPhysicalDevice;
            KRONOS_CORE_ASSERT(m_PhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable physical device!");
        };
        
        void CreateLogicalDevice() {};

    private:
        VulkanConfiguration m_VulkanConfiguration;

        VulkanQueueFamilyIndex m_PresentQueueFamilyIndex;
        VulkanQueueFamilyIndex m_GraphicsQueueFamilyIndex;
    
        VkInstance m_Instance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkPhysicalDevice m_PhysicalDevice;
    };

    class VulkanSwapchain 
    {
    public:
        VulkanSwapchain(VulkanDevice& vulkanDevice)
            : m_VulkanDevice(vulkanDevice)
        {}

    private:
        VulkanDevice m_VulkanDevice;
    };
}