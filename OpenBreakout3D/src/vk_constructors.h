#include "util.h"

namespace OB3D
{
	namespace VkConstructors
	{
		VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queue_family_idx);
		VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool command_pool, uint32_t frame_count);
	}
}