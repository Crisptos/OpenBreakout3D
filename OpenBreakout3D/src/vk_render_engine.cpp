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
        InitSyncStructs();

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

        Destroyable dstr_inst;
        dstr_inst.inst = m_Instance;
        dstr_inst.type = DestroyableVkType::DESTROYABLE_INSTANCE;
        global_queue.Push(dstr_inst);
        Destroyable dstr_dbg;
        dstr_dbg.dbg_msg = m_DbgMessenger;
        dstr_dbg.type = DestroyableVkType::DESTROYABLE_DBG_MESSENGER;
        global_queue.Push(dstr_dbg);

        // Get surface from GLFW
        VkResult result = glfwCreateWindowSurface(m_Instance, m_Window, nullptr, &m_Surface);
        OB3D_VK_CHECK(result, "Failed to create vk surface!");
        fmt::println("SurfaceKHR created successfully");

        Destroyable dstr_surf;
        dstr_surf.surface = m_Surface;
        dstr_surf.type = DestroyableVkType::DESTROYABLE_SURFACE;
        global_queue.Push(dstr_surf);

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

        Destroyable dstr_device;
        dstr_device.device = m_Device.logical;
        dstr_device.type = DestroyableVkType::DESTROYABLE_DEVICE;
        global_queue.Push(dstr_device);

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

    void RenderEngine::InitSyncStructs()
    {
        // create the structures needed to synchronize the CPU and GPU
        // One fence will control when the GPU tells the CPU it's finished rendering a frame
        // Two semaphores will synchronize when we request an image from the swapchain and present an image
        // Start the fence as signaled

        VkFenceCreateInfo fence_create_info = VkConstructors::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
        VkSemaphoreCreateInfo semaphore_create_info = VkConstructors::SemaphoreCreateInfo(0);

        for (int i = 0; i < FRAME_OVERLAP; i++)
        {
            VkResult result = vkCreateFence(m_Device.logical, &fence_create_info, nullptr, &m_Frames[i].render_fence);
            OB3D_VK_CHECK(result, "Failed to create fence for frames");
            result = vkCreateSemaphore(m_Device.logical, &semaphore_create_info, nullptr, &m_Frames[i].swapchain_semaphore);
            OB3D_VK_CHECK(result, "Failed to create semaphore for swapchain");
            result = vkCreateSemaphore(m_Device.logical, &semaphore_create_info, nullptr, &m_Frames[i].render_semaphore);
            OB3D_VK_CHECK(result, "Failed to create semaphore for rendering");
            fmt::println("Created fence and semaphores for frame {}", i);
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
        // Wait until the GPU has finished rendering the last frame. Timeout of 1 sec
        VkResult result = vkWaitForFences(m_Device.logical, 1, &GetCurrentFrame().render_fence, true, 1000000000);
        OB3D_VK_CHECK(result, "Fence timeout!");
        result = vkResetFences(m_Device.logical, 1, &GetCurrentFrame().render_fence);

        // Request image from the swapchain
        // If the swapchain doesn't have any image we can use it will block the
        // calling thread with the timeout specified which is 1 sec (in nanoseconds)
        uint32_t swapchain_img_idx;
        vkAcquireNextImageKHR(m_Device.logical, m_Swapchain, 1000000000, GetCurrentFrame().swapchain_semaphore, nullptr, &swapchain_img_idx);

        VkCommandBuffer cmd_buff = GetCurrentFrame().main_command_buffer;
        result = vkResetCommandBuffer(cmd_buff, 0);
        OB3D_VK_CHECK(result, "Failed to reset command buffer");

        VkCommandBufferBeginInfo cmd_buffer_begin_info = VkConstructors::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        result = vkBeginCommandBuffer(cmd_buff, &cmd_buffer_begin_info);
        OB3D_VK_CHECK(result, "Failed to begin command buffer");

        // make the swapchain image into a writeable mode before rendering
        VkImageFunctions::TransitionImage(cmd_buff, m_SwapchainImages[swapchain_img_idx], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

        VkClearColorValue clear_value = {};
        float flash = std::abs(std::sin(m_FrameCount / 120.0f));
        clear_value = { {0.0f, 0.0f, flash, 1.0f} };

        VkImageSubresourceRange clear_range = VkConstructors::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

        // Clear image
        vkCmdClearColorImage(cmd_buff, m_SwapchainImages[swapchain_img_idx], VK_IMAGE_LAYOUT_GENERAL, &clear_value, 1, &clear_range);

        // make the swapchain image into a presentable mode
        VkImageFunctions::TransitionImage(cmd_buff, m_SwapchainImages[swapchain_img_idx], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        result = vkEndCommandBuffer(cmd_buff);
        OB3D_VK_CHECK(result, "Unable to end command buffer");

        // prepare submissions to the queue
        // we want to wait on the present semaphore, as that semaphore is signaled when the swapchain is ready
        // we will signal the render semaphore to indicate rendering has finished

        VkCommandBufferSubmitInfo cmd_submit_info = VkConstructors::CommandBufferSubmitInfo(cmd_buff);

        VkSemaphoreSubmitInfo wait_info = VkConstructors::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, GetCurrentFrame().swapchain_semaphore);
        VkSemaphoreSubmitInfo signal_info = VkConstructors::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrame().render_semaphore);

        VkSubmitInfo2 submit_info_2 = VkConstructors::SubmitInfo2(&cmd_submit_info, &signal_info, &wait_info);

        // Submit the command buffer to the queue and execute it
        // render fence will now block until the graphics commands finish execution
        result = vkQueueSubmit2(m_GraphicsQueue, 1, &submit_info_2, GetCurrentFrame().render_fence);
        OB3D_VK_CHECK(result, "Failed to submit info to the graphics queue");

        // prepare present
        // put the image we just rendered to into the visible window
        // we want to wait on the render semaphore for that
        // as its necessary that drawing commands have finished before the image is displayed to the user
        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.pNext = nullptr;
        present_info.pSwapchains = &m_Swapchain;
        present_info.swapchainCount = 1;

        present_info.pWaitSemaphores = &GetCurrentFrame().render_semaphore;
        present_info.waitSemaphoreCount = 1;

        present_info.pImageIndices = &swapchain_img_idx;

        result = vkQueuePresentKHR(m_GraphicsQueue, &present_info);
        OB3D_VK_CHECK(result, "Failed to present to the graphics queue!");

        // increment frame number
        m_FrameCount++;
    }

    void RenderEngine::Destroy()
    {
        if (m_IsInitialized)
        {
            fmt::println("Shutting down...");
            // Reverse order of creation
            loaded_engine = nullptr;
            // Vulkan
            //  Rendering
            vkDeviceWaitIdle(m_Device.logical);
            for (int i = 0; i < FRAME_OVERLAP; i++)
            {
                vkDestroyCommandPool(m_Device.logical, m_Frames[i].command_pool, nullptr);

                // sync objects
                vkDestroyFence(m_Device.logical, m_Frames[i].render_fence, nullptr);
                vkDestroySemaphore(m_Device.logical, m_Frames[i].render_semaphore, nullptr);
                vkDestroySemaphore(m_Device.logical, m_Frames[i].swapchain_semaphore, nullptr);
            }

            vkDestroySwapchainKHR(m_Device.logical, m_Swapchain, nullptr);
            for (int i = 0; i < m_SwapchainImageViews.size(); i++)
            {
                vkDestroyImageView(m_Device.logical, m_SwapchainImageViews[i], nullptr);
            }

            //  Core
            global_queue.Flush();
            //vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            //vkDestroyDevice(m_Device.logical, nullptr);
            //vkb::destroy_debug_utils_messenger(m_Instance, m_DbgMessenger);
            //vkDestroyInstance(m_Instance, nullptr);
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