#pragma once
#include "util.h"
#include "vk_constructors.h"
#include "vk_image_functions.h"
#include "destroyer_queue.h"

namespace OB3D
{
    struct AllocatedImage
    {
        VkImage img;
        VkImageView img_view;
        VmaAllocation alloc;
        VkExtent3D img_ext;
        VkFormat img_format;
    };

    struct FrameData
    {

        VkCommandPool command_pool;
        VkCommandBuffer main_command_buffer;
        VkSemaphore swapchain_semaphore, render_semaphore;
        VkFence render_fence;
        DestroyerQueue frame_queue;
    };

    constexpr unsigned int FRAME_OVERLAP = 2;

    class RenderEngine
    {
        // Functions
    public:
        static RenderEngine &Get();
        FrameData& GetCurrentFrame();

        void Init();
        void Run();
        void Draw();
        void Destroy();

    private:
        // Initialization
        void InitVulkan();
        void CreateSwapchain(uint32_t width, uint32_t height);
        void InitSwapchain();
        void InitCommands();
        void InitSyncStructs();

        // Rendering
        void DrawBackground(VkCommandBuffer cmd);

        // Class Members
    public:
        bool m_IsInitialized = false;
        int m_FrameCount = 0;
        bool m_IsIdle = false;

    private:
        // Engine Util
        DestroyerQueue global_queue;
        VmaAllocator m_VmaAlloc;

        // GLFW
        struct GLFWwindow *m_Window;
        uint32_t m_Width;
        uint32_t m_Height;

        // Vulkan
        //  Core
        VkInstance m_Instance;
        struct RenderDevice
        {
            VkPhysicalDevice physical;
            VkDevice logical;
        } m_Device;
        VkDebugUtilsMessengerEXT m_DbgMessenger;
        VkSurfaceKHR m_Surface;
        
        //  Rendering
        VkSwapchainKHR m_Swapchain;
        VkFormat m_SwapchainImageFormat;
        std::vector<VkImage> m_SwapchainImages;
        std::vector<VkImageView> m_SwapchainImageViews;
        VkExtent2D m_SwapchainExtent;

        // Offline rendering image
        AllocatedImage m_DrawImg;
        VkExtent2D m_DrawExt;

        FrameData m_Frames[FRAME_OVERLAP];
        VkQueue m_GraphicsQueue;
        uint32_t m_GraphicsQueueFamilyIdx;
    };
}