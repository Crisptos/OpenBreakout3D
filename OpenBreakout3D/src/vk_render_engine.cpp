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
        m_IsInitialized = true;
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

            loaded_engine = nullptr;
            glfwDestroyWindow(m_Window);
            glfwTerminate();
        }
    }

    RenderEngine &RenderEngine::Get()
    {
        return *loaded_engine;
    }
}