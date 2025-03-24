#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <VkBootstrap.h>
#include "vk_render_engine.h"

namespace OB3D
{
    RenderEngine *loaded_engine = nullptr;

    void RenderEngine::Init()
    {
        assert(loaded_engine == nullptr);
        loaded_engine = this;

        if (!glfwInit())
        {
            OB3D_ERROR_OUT("GLFW failed to initialize");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        m_Window = glfwCreateWindow(
            800,
            600,
            "Open Breakout 3D",
            nullptr,
            nullptr);

        if (!m_Window)
        {
            OB3D_ERROR_OUT("GLFW failed to create window");
        }

        // Vulkan Initialization
        InitVulkan();
        InitSwapchain();
        InitCommands();

        m_IsInitialized = true;
    }

    void RenderEngine::InitVulkan()
    {
        // Instance and Messenger
        vkb::InstanceBuilder instance_builder;
        auto built_inst = instance_builder.set_app_name("OpenBreakout3D")
                              .request_validation_layers(true)
                              .use_default_debug_messenger()
                              .require_api_version(1, 4, 0)
                              .build();

        if (!built_inst)
        {
            OB3D_ERROR_OUT("Failed to create vk instance!");
        }
        vkb::Instance vkb_inst = built_inst.value();
        fmt::println("VkInstance created successfully");

        // Grab instance from vkb and instantiate the Vulkan one
        m_Instance = vkb_inst.instance;
        m_DbgMessenger = vkb_inst.debug_messenger;

        // Get surface from GLFW
        VkResult result = glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface);
        OB3D_VK_CHECK(result, "Failed to create vk surface!");
        fmt::println("SurfaceKHR created successfully");

        // Physical Device
        // Grab features for Vulkan 1.3 and 1.2
        VkPhysicalDeviceVulkan13Features features13 = {};
        features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        features13.dynamicRendering = true;
        features13.synchronization2 = true;

        VkPhysicalDeviceVulkan12Features features12 = {};
        features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing = true;

        vkb::PhysicalDeviceSelector physical_selector(vkb_inst);
        vkb::PhysicalDevice selected_physical = physical_selector.set_minimum_version(1, 4)
                                                    .set_required_features_13(features13)
                                                    .set_required_features_12(features12)
                                                    .set_surface(m_Surface)
                                                    .select()
                                                    .value();

        vkb::DeviceBuilder device_builder(selected_physical);
        vkb::Device built_device = device_builder.build().value();
        m_Device.logical = built_device.device;
        m_Device.physical = selected_physical.physical_device;
        fmt::println("Device successfully chosen: {:s}", selected_physical.name);

        // Use vk-bootstrap to get a Graphics queue
        m_GraphicsQueue = built_device.get_queue(vkb::QueueType::graphics).value();
        m_GraphicsQueueFamilyIdx = built_device.get_queue_index(vkb::QueueType::graphics).value();
    }

    void RenderEngine::InitSwapchain()
    {
        CreateSwapchain(800, 600);
        fmt::println("SwapchainKHR created successfully");
    }

    void RenderEngine::CreateSwapchain(uint32_t width, uint32_t height)
    {
        vkb::SwapchainBuilder swapchain_builder(m_Device.physical, m_Device.logical, m_Surface);
        m_SwapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;
        VkSurfaceFormatKHR desired_format = {};
        desired_format.format = m_SwapchainImageFormat;
        desired_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        vkb::Swapchain built_swapchain = swapchain_builder.set_desired_format(desired_format)
                                             .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
                                             .set_desired_extent(width, height)
                                             .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
                                             .build()
                                             .value();

        m_SwapchainExtent = built_swapchain.extent;

        // Store the swapchain and its images
        m_Swapchain = built_swapchain.swapchain;
        m_SwapchainImages = built_swapchain.get_images().value();
        m_SwapchainImageViews = built_swapchain.get_image_views().value();
    }

    void RenderEngine::InitCommands()
    {
        // Create a command pool for commands submitted to the graphics queue
        // we also want the pool to allow for resetting of individual command buffers
        VkCommandPoolCreateInfo command_pool_create_info = VkConstructors::CommandPoolCreateInfo(m_GraphicsQueueFamilyIdx);

        for (int i = 0; i < FRAME_OVERLAP; i++)
        {
            VkResult result = vkCreateCommandPool(m_Device.logical, &command_pool_create_info, nullptr, &m_Frames[i].command_pool);
            OB3D_VK_CHECK(result, "Failed to create command pool!");

            // Allocate the default command buffer that we will use for rendering
            VkCommandBufferAllocateInfo command_buffer_allocate_info = VkConstructors::CommandBufferAllocateInfo(m_Frames[i].command_pool, 1);

            result = vkAllocateCommandBuffers(m_Device.logical, &command_buffer_allocate_info, &m_Frames[i].main_command_buffer);
            OB3D_VK_CHECK(result, "Failed to allocate command buffers!");

            fmt::println("Successfully created frame data {} with the command pool and a unique command buffer", i);
        }
    }

    void RenderEngine::Run()
    {
        while (!glfwWindowShouldClose(m_Window))
        {
            if (!m_IsIdle)
            {
                Draw();
            }

            glfwPollEvents();
        }
    }

    void RenderEngine::Draw()
    {
    }

    void RenderEngine::Destroy()
    {
        if (m_IsInitialized)
        {
            // Reverse order of creation
            loaded_engine = nullptr;
            // Vulkan
            //  Rendering
            vkDeviceWaitIdle(m_Device.logical);
            for (int i = 0; i < FRAME_OVERLAP; i++)
            {
                vkDestroyCommandPool(m_Device.logical, m_Frames[i].command_pool, nullptr);
            }

            vkDestroySwapchainKHR(m_Device.logical, m_Swapchain, nullptr);
            for (int i = 0; i < m_SwapchainImageViews.size(); i++)
            {
                vkDestroyImageView(m_Device.logical, m_SwapchainImageViews[i], nullptr);
            }

            //  Core
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            vkDestroyDevice(m_Device.logical, nullptr);
            vkb::destroy_debug_utils_messenger(m_Instance, m_DbgMessenger);
            vkDestroyInstance(m_Instance, nullptr);
            // GLFW
            glfwDestroyWindow(m_Window);
            glfwTerminate();
        }
    }

    RenderEngine &RenderEngine::Get()
    {
        return *loaded_engine;
    }

    FrameData& RenderEngine::GetCurrentFrame()
    {
        return m_Frames[m_FrameCount % FRAME_OVERLAP];
    }
}