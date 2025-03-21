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

        m_IsInitialized = true;
    }

    void RenderEngine::InitVulkan()
    {
        // Instance and Messenger
        vkb::InstanceBuilder instance_builder;
        auto built_inst = instance_builder.set_app_name("OpenBreakout3D")
                              .request_validation_layers(true)
                              .use_default_debug_messenger()
                              .require_api_version(1, 3, 0)
                              .build();

        if(!built_inst)
        {
            OB3D_ERROR_OUT("Failed to create vk instance");
        }
        vkb::Instance vkb_inst = built_inst.value();
        fmt::println("VkInstance created successfully");

        // Grab instance from vkb and instantiate the Vulkan one
        m_Instance = vkb_inst.instance;
        m_DbgMessenger = vkb_inst.debug_messenger;

        // Get surface from GLFW
        VkResult result = glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface);
        if(result != VK_SUCCESS)
        {
            OB3D_ERROR_OUT("Failed to create vk surface");
        }
        fmt::println("Created SurfaceKHR successfully");

        // Physical Device
        // Grab features for Vulkan 1.3 and 1.2
        VkPhysicalDeviceVulkan13Features features13;
        features13.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        features13.dynamicRendering = true;
        features13.synchronization2 = true;

        VkPhysicalDeviceVulkan12Features features12;
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
}