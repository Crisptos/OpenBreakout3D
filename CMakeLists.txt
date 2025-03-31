cmake_minimum_required(VERSION 3.15)
project(OpenBreakout3D)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(Vendor/GLFW)
add_subdirectory(Vendor/GLM)
add_subdirectory(Vendor/vk-bootstrap)
add_subdirectory(Vendor/VulkanMemoryAllocator)
add_subdirectory(Vendor/fmt)
add_subdirectory(Shaders)
add_subdirectory(OpenBreakout3D)

# Ensure the shaders build before the main target
add_dependencies(${PROJECT_NAME} Shaders)