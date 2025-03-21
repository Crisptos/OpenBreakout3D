#pragma once
#include "util.h"

namespace OB3D
{
    class RenderEngine
    {
        // Functions
    public:
        static RenderEngine &Get();

        void Init();
        void Run();
        void Draw();
        void Destroy();

    private:
        void InitVulkan();

        // Class Members
    public:
        bool m_IsInitialized = false;
        int m_FrameIdx = 0;
        bool m_IsIdle = false;

    private:
        // GLFW
        struct GLFWwindow *m_Window;

        // Vulkan
        VkInstance m_Instance;
        struct RenderDevice
        {
            VkPhysicalDevice physical;
            VkDevice logical;
        } m_Device;
        VkDebugUtilsMessengerEXT m_DbgMessenger;
        VkSurfaceKHR m_Surface;
    };
}