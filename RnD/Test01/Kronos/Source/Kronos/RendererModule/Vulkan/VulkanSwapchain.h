#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#include <vector>
#include <set>

namespace Kronos
{
    class VulkanDevice;

    struct SwapchainProperties
    {
	    VkSwapchainKHR                OldSwapchain;
	    uint32_t                      ImageCount{3};
	    VkExtent2D                    Extent{};
	    VkSurfaceFormatKHR            SurfaceFormat{};
	    uint32_t                      ArrayLayers;
	    VkImageUsageFlags             ImageUsage;
	    VkSurfaceTransformFlagBitsKHR PreTransform;
	    VkCompositeAlphaFlagBitsKHR   CompositeAlpha;
	    VkPresentModeKHR              PresentMode;
    };

    class VulkanSwapchain
    {
    public:
        VulkanSwapchain(
            VulkanDevice &device,
            VkSurfaceKHR surface,
            const VkExtent2D extent = {},
            const uint32_t imageCount = 3,
            const VkSurfaceTransformFlagBitsKHR transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            const VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR,
            const std::set<VkImageUsageFlagBits> &image_usage_flags = {VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT}    );

        VulkanSwapchain(
            VulkanSwapchain &oldSwapchain,
            VulkanDevice &device,
            VkSurfaceKHR surface,
            const VkExtent2D extent = {},
            const uint32_t imageCount = 3,
            const VkSurfaceTransformFlagBitsKHR transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
            const VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR,
            const std::set<VkImageUsageFlagBits> &image_usage_flags = {VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT});

        VkSwapchainKHR GetHandle() const;

        const std::vector<VkImage>& GetImages() const;

        const VkExtent2D& GetExtent() const;

        VkFormat GetSurfaceFormat() const;

    private:
        VulkanDevice &m_Device;
        VkSurfaceKHR m_Surface{VK_NULL_HANDLE};
        VkSwapchainKHR m_Handle{VK_NULL_HANDLE};
        std::vector<VkImage> m_Images;
        std::vector<VkSurfaceFormatKHR> m_AvailableSurfaceFormats{};
        std::vector<VkPresentModeKHR> m_AvailablePresentModes{};
        SwapchainProperties m_Properties;
        std::set<VkImageUsageFlagBits> m_ImageUsageFlags;

        const std::vector<VkPresentModeKHR> m_PresentModePriorityList = {
            VK_PRESENT_MODE_FIFO_KHR,
            VK_PRESENT_MODE_MAILBOX_KHR};

        const std::vector<VkSurfaceFormatKHR> m_SurfaceFormatPriorityList = {
            {VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            {VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            {VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
            {VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}};
    };
}