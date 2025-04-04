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
		VkImageCreateInfo ImageCreateInfo(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D ext);
		VkImageViewCreateInfo ImageViewCreateInfo(VkFormat format, VkImage img, VkImageAspectFlags aspect_flags);

		struct DescriptorLayoutBuilder
		{
			std::vector<VkDescriptorSetLayoutBinding> bindings;

			void AddBinding(uint32_t binding, VkDescriptorType type);
			void Clear();
			VkDescriptorSetLayout Build(VkDevice device, VkShaderStageFlags shader_stages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
		};

		struct DescriptorAllocator
		{
			struct PoolSizeRatio
			{
				VkDescriptorType type;
				float ratio;
			};

			VkDescriptorPool pool;
			
			void InitPool(VkDevice device, uint32_t max_sets, std::span<PoolSizeRatio> pool_ratios);
			void ClearDescriptors(VkDevice device);
			void DestroyPool(VkDevice device);

			VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout);
		};
	}
}