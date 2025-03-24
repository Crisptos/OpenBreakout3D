#include "vk_constructors.h"

namespace OB3D
{
	namespace VkConstructors
	{
		VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queue_family_idx)
		{
			VkCommandPoolCreateInfo command_pool_create_info = {};
			command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			command_pool_create_info.pNext = nullptr;
			command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			command_pool_create_info.queueFamilyIndex = queue_family_idx;

			return command_pool_create_info;
		}

		VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool command_pool, uint32_t frame_count)
		{
			VkCommandBufferAllocateInfo command_buffer_allocate_info = {};
			command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			command_buffer_allocate_info.pNext = nullptr;
			command_buffer_allocate_info.commandPool = command_pool;
			command_buffer_allocate_info.commandBufferCount = frame_count;
			command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

			return command_buffer_allocate_info;
		}

	}
}