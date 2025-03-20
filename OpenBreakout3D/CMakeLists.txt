find_package(Vulkan)
file(GLOB_RECURSE OBSOURCES src/*.cpp src/*.h)

add_executable (OpenBreakout3D ${OBSOURCES})

target_link_libraries(OpenBreakout3D PRIVATE Vulkan::Vulkan glfw glm::glm vk-bootstrap::vk-bootstrap GPUOpen::VulkanMemoryAllocator fmt::fmt)

# TODO: Add tests and install targets if needed.