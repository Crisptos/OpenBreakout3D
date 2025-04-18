#pragma once
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <span>
#include <array>
#include <functional>
#include <deque>

#include <vulkan/vulkan.h>
#include <vulkan/vk_enum_string_helper.h>
#include <vk_mem_alloc.h>

#include <fmt/core.h>

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#define OB3D_ERROR_OUT(s) fmt::println("{:s}", s); abort();
#define OB3D_VK_CHECK(r, s) if(r != VK_SUCCESS){ OB3D_ERROR_OUT(s); }