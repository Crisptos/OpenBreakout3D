#include "util.h"

namespace OB3D
{
	namespace VkConstructors
	{
		VkCommandPoolCreateInfo CommandPoolCreateInfo(uint32_t queue_family_idx);
		VkCommandBufferAllocateInfo CommandBufferAllocateInfo(VkCommandPool command_pool, uint32_t frame_count);
		VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags);
		VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags);
		VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags);
		VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspect_mask);
		VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags stage_mask, VkSemaphore semaphore);
		VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer cmd_buff);
		VkSubmitInfo2 SubmitInfo2(VkCommandBufferSubmitInfo* cmd_buff_submit_info, VkSemaphoreSubmitInfo* signal_semaphore_info, VkSemaphoreSubmitInfo* wait_semaphore_info);
	}
}