#include "Kronos/RendererModule/Vulkan/VulkanSwapchain.h"
#include "Kronos/RendererModule/Vulkan/VulkanDevice.h"
#include "Kronos/RendererModule/Vulkan/VulkanPhysicalDevice.h"

#include "Kronos/LoggingModule/LoggingModule.h"

#include <algorithm>

namespace Kronos
{
    inline uint32_t ChooseImageCount(uint32_t requestedImageCount, uint32_t minImageCount, uint32_t maxImageCount)
    {
        if (maxImageCount != 0)
        {
            requestedImageCount = (std::min)(requestedImageCount, maxImageCount);
        }

        requestedImageCount = (std::max)(requestedImageCount, minImageCount);

        return requestedImageCount;
    }

    inline uint32_t ChooseImageArrayLayers(uint32_t requestImageArrayLayers, uint32_t maxImageArrayLayers)
    {
        requestImageArrayLayers = (std::min)(requestImageArrayLayers, maxImageArrayLayers);
        requestImageArrayLayers = (std::max)(requestImageArrayLayers, 1u);

        return requestImageArrayLayers;
    }

    inline VkExtent2D ChooseExtent(VkExtent2D requestExtent, const VkExtent2D &minImageExtent, const VkExtent2D &maxImageExtent, const VkExtent2D &currentExtent)
    {
        if (currentExtent.width == 0xFFFFFFFF)
        {
            return requestExtent;
        }

        if (requestExtent.width < 1 || requestExtent.height < 1)
        {
            Log::Trace("(Swapchain) Image extent ({}, {}) not supported. Selecting ({}, {})." /*, requestExtent.width, requestExtent.height, currentExtent.width, currentExtent.height*/);
            return currentExtent;
        }

        requestExtent.width = (std::max)(requestExtent.width, minImageExtent.width);
        requestExtent.width = (std::min)(requestExtent.width, maxImageExtent.width);

        requestExtent.height = (std::max)(requestExtent.height, minImageExtent.height);
        requestExtent.height = (std::min)(requestExtent.height, maxImageExtent.height);

        return requestExtent;
    }

    inline VkPresentModeKHR ChoosePresentMode(VkPresentModeKHR requestPresentMode, const std::vector<VkPresentModeKHR> &availablePresentModes, const std::vector<VkPresentModeKHR> &presentModePriorityList)
    {
        auto presentModeIt = std::find(availablePresentModes.begin(), availablePresentModes.end(), requestPresentMode);

        if (presentModeIt == availablePresentModes.end())
        {
            // If nothing found, always default to FIFO
            VkPresentModeKHR chosenPresentMode = VK_PRESENT_MODE_FIFO_KHR;

            for (auto &presentMode : presentModePriorityList)
            {
                if (std::find(availablePresentModes.begin(), availablePresentModes.end(), presentMode) != availablePresentModes.end())
                {
                    chosenPresentMode = presentMode;
                }
            }

            Log::Trace("(Swapchain) Present mode '{}' not supported. Selecting '{}'." /*, to_string(requestPresentMode), to_string(chosenPresentMode)*/);
            return chosenPresentMode;
        }
        else
        {
            Log::Trace("(Swapchain) Present mode selected: {}" /*, to_string(requestPresentMode)*/);
            return *presentModeIt;
        }
    }

    inline VkSurfaceFormatKHR ChooseSurfaceFormat(const VkSurfaceFormatKHR requestedSurfaceFormat, const std::vector<VkSurfaceFormatKHR> &availableSurfaceFormats, const std::vector<VkSurfaceFormatKHR> &surfaceFormatPriorityList)
    {
        // Try to find the requested surface format in the supported surface formats
        auto surfaceFormatIt = std::find_if(availableSurfaceFormats.begin(), availableSurfaceFormats.end(), [&requestedSurfaceFormat](const VkSurfaceFormatKHR &surface)
                                            {
                if (surface.format == requestedSurfaceFormat.format &&
                    surface.colorSpace == requestedSurfaceFormat.colorSpace)
                {
                    return true;
                }

                return false; });

        // If the requested surface format isn't found, then try to request a format from the priority list
        if (surfaceFormatIt == availableSurfaceFormats.end())
        {
            for (auto &surfaceFormat : surfaceFormatPriorityList)
            {
                surfaceFormatIt = std::find_if(
                    availableSurfaceFormats.begin(),
                    availableSurfaceFormats.end(),
                    [&surfaceFormat](const VkSurfaceFormatKHR &surface)
                    {
                        if (surface.format == surfaceFormat.format &&
                            surface.colorSpace == surfaceFormat.colorSpace)
                        {
                            return true;
                        }

                        return false;
                    });
                if (surfaceFormatIt != availableSurfaceFormats.end())
                {
                    Log::Trace("(Swapchain) Surface format ({}) not supported. Selecting ({})." /* , to_string(requestedSurfaceFormat), to_string(*surfaceFormatIt) */);
                    return *surfaceFormatIt;
                }
            }

            // If nothing found, default the first supporte surface format
            surfaceFormatIt = availableSurfaceFormats.begin();
            Log::Trace("(Swapchain) Surface format ({}) not supported. Selecting ({})." /*, to_string(requestedSurfaceFormat), to_string(*surfaceFormatIt)*/);
        }
        else
        {
            Log::Trace("(Swapchain) Surface format selected: {}" /* , to_string(requestedSurfaceFormat) */);
        }

        return *surfaceFormatIt;
    }

    inline VkSurfaceTransformFlagBitsKHR ChooseTransform(VkSurfaceTransformFlagBitsKHR requestTransform, VkSurfaceTransformFlagsKHR supportedTransform, VkSurfaceTransformFlagBitsKHR currentTransform)
    {
        if (requestTransform & supportedTransform)
        {
            return requestTransform;
        }

        Log::Trace("(Swapchain) Surface transform '{}' not supported. Selecting '{}'." /* , to_string(request_transform), to_string(current_transform) */);

        return currentTransform;
    }

    inline VkCompositeAlphaFlagBitsKHR ChooseCompositeAlpha(VkCompositeAlphaFlagBitsKHR requestCompositeAlpha, VkCompositeAlphaFlagsKHR supportedCompositeAlpha)
    {
        if (requestCompositeAlpha & supportedCompositeAlpha)
        {
            return requestCompositeAlpha;
        }

        static const std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
            VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
            VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
            VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR};

        for (VkCompositeAlphaFlagBitsKHR compositeAlpha : compositeAlphaFlags)
        {
            if (compositeAlpha & supportedCompositeAlpha)
            {
                Log::Trace("(Swapchain) Composite alpha '{}' not supported. Selecting '{}." /* , to_string(requestCompositeAlpha), to_string(composite_alpha) */);
                return compositeAlpha;
            }
        }

        throw std::runtime_error("No compatible composite alpha found."); // TODO: Make an engine solution for asserts and errors, and write a quick guide for when to use what.
    }

    inline bool ValidateFormatFeature(VkImageUsageFlagBits imageUsage, VkFormatFeatureFlags supportedFeatures)
    {
        switch (imageUsage)
        {
        case VK_IMAGE_USAGE_STORAGE_BIT:
            return VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT & supportedFeatures;
        default:
            return true;
        }
    }

    inline std::set<VkImageUsageFlagBits> ChooseImageUsage(const std::set<VkImageUsageFlagBits> &requestedImageUsageFlags, VkImageUsageFlags supportedImageUsage, VkFormatFeatureFlags supportedFeatures)
    {
        std::set<VkImageUsageFlagBits> validatedImageUsageFlags;
        for (auto flag : requestedImageUsageFlags)
        {
            if ((flag & supportedImageUsage) && ValidateFormatFeature(flag, supportedFeatures))
            {
                validatedImageUsageFlags.insert(flag);
            }
            else
            {
                Log::Trace("(Swapchain) Image usage ({}) requested but not supported." /* , to_string(flag) */);
            }
        }

        if (validatedImageUsageFlags.empty())
        {
            // Pick the first format from list of defaults, if supported
            static const std::vector<VkImageUsageFlagBits> imageUsageFlags = {
                VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
                VK_IMAGE_USAGE_STORAGE_BIT,
                VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_IMAGE_USAGE_TRANSFER_DST_BIT};

            for (VkImageUsageFlagBits imageUsage : imageUsageFlags)
            {
                if ((imageUsage & supportedImageUsage) && ValidateFormatFeature(imageUsage, supportedFeatures))
                {
                    validatedImageUsageFlags.insert(imageUsage);
                    break;
                }
            }
        }

        if (!validatedImageUsageFlags.empty())
        {
            // TODO:
            // Log image usage flags used
            /*std::string usageList;
            for (VkImageUsageFlagBits imageUsage : validatedImageUsageFlags)
            {
                usageList += to_string(imageUsage) + " ";
            }
            Log::Trace("(Swapchain) Image usage flags: {}" , usageList ); */
            Log::Trace("TODO: usage list");
        }
        else
        {
            throw std::runtime_error("No compatible image usage found.");
        }

        return validatedImageUsageFlags;
    }

    inline VkImageUsageFlags CompositeImageFlags(std::set<VkImageUsageFlagBits> &imageUsageFlags)
    {
        VkImageUsageFlags imageUsage{};
        for (auto flag : imageUsageFlags)
        {
            imageUsage |= flag;
        }
        return imageUsage;
    }

    VulkanSwapchain::VulkanSwapchain(
        VulkanDevice &device,
        VkSurfaceKHR surface,
        const VkExtent2D extent,
        const uint32_t imageCount,
        const VkSurfaceTransformFlagBitsKHR transform,
        const VkPresentModeKHR presentMode,
        const std::set<VkImageUsageFlagBits> &imageUsageFlags)
        : VulkanSwapchain{*this, device, surface, extent, imageCount, transform, presentMode, imageUsageFlags}
    {   
    }

    VulkanSwapchain::VulkanSwapchain(
        VulkanSwapchain &oldSwapchain,
        VulkanDevice &device,
        VkSurfaceKHR surface,
        const VkExtent2D extent,
        const uint32_t imageCount,
        const VkSurfaceTransformFlagBitsKHR transform,
        const VkPresentModeKHR presentMode,
        const std::set<VkImageUsageFlagBits> &imageUsageFlags)
        : m_Device(device), m_Surface(surface)
    {
        // m_PresentModePriorityList = oldSwapchain.m_PresentModePriorityList;
        // m_SurfaceFormatPriorityList = oldSwapchain.m_SurfaceFormatPriorityList;

        VkSurfaceCapabilitiesKHR surfaceCapabilities{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_Device.GetPhysicalDevice().GetHandle(), surface, &surfaceCapabilities);

        uint32_t surfaceFormatCount{0U};
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device.GetPhysicalDevice().GetHandle(), surface, &surfaceFormatCount, nullptr);
        m_AvailableSurfaceFormats.resize(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_Device.GetPhysicalDevice().GetHandle(), surface, &surfaceFormatCount, m_AvailableSurfaceFormats.data());

        uint32_t presentModeCount{0U};
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device.GetPhysicalDevice().GetHandle(), surface, &presentModeCount, nullptr);
        m_AvailablePresentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_Device.GetPhysicalDevice().GetHandle(), surface, &presentModeCount, m_AvailablePresentModes.data());

        m_Properties.ImageCount = ChooseImageCount(imageCount, surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);
        m_Properties.Extent = ChooseExtent(extent, surfaceCapabilities.minImageExtent, surfaceCapabilities.maxImageExtent, surfaceCapabilities.currentExtent);
        m_Properties.ArrayLayers = ChooseImageArrayLayers(1U, surfaceCapabilities.maxImageArrayLayers);
        m_Properties.SurfaceFormat = ChooseSurfaceFormat(m_Properties.SurfaceFormat, m_AvailableSurfaceFormats, m_SurfaceFormatPriorityList);
        VkFormatProperties FormatProperties;
        vkGetPhysicalDeviceFormatProperties(m_Device.GetPhysicalDevice().GetHandle(), m_Properties.SurfaceFormat.format, &FormatProperties);
        m_ImageUsageFlags = ChooseImageUsage(m_ImageUsageFlags, surfaceCapabilities.supportedUsageFlags, FormatProperties.optimalTilingFeatures);
        m_Properties.ImageUsage = CompositeImageFlags(m_ImageUsageFlags);
        m_Properties.PreTransform = ChooseTransform(transform, surfaceCapabilities.supportedTransforms, surfaceCapabilities.currentTransform);
        m_Properties.CompositeAlpha = ChooseCompositeAlpha(VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR, surfaceCapabilities.supportedCompositeAlpha);

        m_Properties.OldSwapchain = oldSwapchain.GetHandle();
        m_Properties.PresentMode = ChoosePresentMode(presentMode, m_AvailablePresentModes, m_PresentModePriorityList);
    
        // Create the swapchain
        VkSwapchainCreateInfoKHR swapchainCreateInfo{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
        swapchainCreateInfo.minImageCount = m_Properties.ImageCount;
        swapchainCreateInfo.imageExtent = m_Properties.Extent;
        swapchainCreateInfo.presentMode = m_Properties.PresentMode;
        swapchainCreateInfo.imageFormat = m_Properties.SurfaceFormat.format;
        swapchainCreateInfo.imageColorSpace = m_Properties.SurfaceFormat.colorSpace;
        swapchainCreateInfo.imageArrayLayers = m_Properties.ArrayLayers;
        swapchainCreateInfo.imageUsage = m_Properties.ImageUsage;
        swapchainCreateInfo.preTransform = m_Properties.PreTransform;
        swapchainCreateInfo.compositeAlpha = m_Properties.CompositeAlpha;
        swapchainCreateInfo.oldSwapchain = m_Properties.OldSwapchain;
        swapchainCreateInfo.surface = m_Surface;

        KRONOS_CORE_ASSERT(vkCreateSwapchainKHR(m_Device.GetHandle(), &swapchainCreateInfo, nullptr, &m_Swapchain) == VK_SUCCESS, "Failed to create swapchain");

        uint32_t imageAvailable{0u};
        vkGetSwapchainImagesKHR(m_Device.GetHandle(), m_Swapchain, &imageAvailable, nullptr);
        m_Images.resize(imageAvailable);
        vkGetSwapchainImagesKHR(m_Device.GetHandle(), m_Swapchain, &imageAvailable, m_Images.data());
    }

    VkSwapchainKHR VulkanSwapchain::GetHandle() const { return m_Swapchain; }

    const std::vector<VkImage>& VulkanSwapchain::GetImages() const { return m_Images; };


    const VkExtent2D& VulkanSwapchain::GetExtent() const
    {
        return m_Properties.Extent;
    }

    VkFormat VulkanSwapchain::GetSurfaceFormat() const
    {
        return m_Properties.SurfaceFormat.format;
    }

    VkResult VulkanSwapchain::AcquireNextImage(uint32_t &imageIndex, VkSemaphore imageAcquiredSemaphore, VkFence fence)
    {
        return vkAcquireNextImageKHR(m_Device.GetHandle(), m_Swapchain, UINT64_MAX, imageAcquiredSemaphore, fence, &imageIndex);
    }
}