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

		VkFenceCreateInfo FenceCreateInfo(VkFenceCreateFlags flags)
		{
			VkFenceCreateInfo fence_create_info = {};
			fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fence_create_info.pNext = nullptr;
			fence_create_info.flags = flags;

			return fence_create_info;
		}

		VkSemaphoreCreateInfo SemaphoreCreateInfo(VkSemaphoreCreateFlags flags)
		{
			VkSemaphoreCreateInfo semaphore_create_info = {};
			semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			semaphore_create_info.pNext = nullptr;
			semaphore_create_info.flags = flags;

			return semaphore_create_info;
		}

		VkCommandBufferBeginInfo CommandBufferBeginInfo(VkCommandBufferUsageFlags flags)
		{
			VkCommandBufferBeginInfo cmd_buffer_begin_info = {};
			cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			cmd_buffer_begin_info.pNext = nullptr;
			cmd_buffer_begin_info.pInheritanceInfo = nullptr;
			cmd_buffer_begin_info.flags = flags;

			return cmd_buffer_begin_info;
		}

		VkImageSubresourceRange ImageSubresourceRange(VkImageAspectFlags aspect_mask)
		{
			VkImageSubresourceRange sub_img = {};
			sub_img.aspectMask = aspect_mask;
			sub_img.baseMipLevel = 0;
			sub_img.levelCount = VK_REMAINING_MIP_LEVELS;
			sub_img.baseArrayLayer = 0;
			sub_img.layerCount = VK_REMAINING_ARRAY_LAYERS;

			return sub_img;
		}

		VkSemaphoreSubmitInfo SemaphoreSubmitInfo(VkPipelineStageFlags stage_mask, VkSemaphore semaphore)
		{
			VkSemaphoreSubmitInfo semaphore_submit_info = {};
			semaphore_submit_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
			semaphore_submit_info.pNext = nullptr;
			semaphore_submit_info.semaphore = semaphore;
			semaphore_submit_info.stageMask = stage_mask;
			semaphore_submit_info.deviceIndex = 0;
			semaphore_submit_info.value = 1;

			return semaphore_submit_info;
		}

		VkCommandBufferSubmitInfo CommandBufferSubmitInfo(VkCommandBuffer cmd_buff)
		{
			VkCommandBufferSubmitInfo cmd_buff_submit_info = {};
			cmd_buff_submit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
			cmd_buff_submit_info.pNext = nullptr;
			cmd_buff_submit_info.commandBuffer = cmd_buff;
			cmd_buff_submit_info.deviceMask = 0;

			return cmd_buff_submit_info;

		}

		VkSubmitInfo2 SubmitInfo2(VkCommandBufferSubmitInfo* cmd_buff_submit_info, VkSemaphoreSubmitInfo* signal_semaphore_info, VkSemaphoreSubmitInfo* wait_semaphore_info)
		{
			VkSubmitInfo2 submit_info_2 = {};
			submit_info_2.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
			submit_info_2.pNext = nullptr;

			submit_info_2.waitSemaphoreInfoCount = wait_semaphore_info == nullptr ? 0 : 1;
			submit_info_2.pWaitSemaphoreInfos = wait_semaphore_info;

			submit_info_2.signalSemaphoreInfoCount = signal_semaphore_info == nullptr ? 0 : 1;
			submit_info_2.pSignalSemaphoreInfos = signal_semaphore_info;

			submit_info_2.commandBufferInfoCount = 1;
			submit_info_2.pCommandBufferInfos = cmd_buff_submit_info;

			return submit_info_2;
		}

		VkImageCreateInfo ImageCreateInfo(VkFormat format, VkImageUsageFlags usage_flags, VkExtent3D ext)
		{
			VkImageCreateInfo create_info_img = {};
			create_info_img.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			create_info_img.pNext = nullptr;

			create_info_img.imageType = VK_IMAGE_TYPE_2D;

			create_info_img.format = format;
			create_info_img.extent = ext;

			create_info_img.mipLevels = 1;
			create_info_img.arrayLayers = 1;

			// for MSAA we will not be using it by default
			create_info_img.samples = VK_SAMPLE_COUNT_1_BIT;

			create_info_img.tiling = VK_IMAGE_TILING_OPTIMAL;
			create_info_img.usage = usage_flags;

			return create_info_img;
		}

		VkImageViewCreateInfo ImageViewCreateInfo(VkFormat format, VkImage img, VkImageAspectFlags aspect_flags)
		{
			VkImageViewCreateInfo create_info_img_view = {};
			create_info_img_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info_img_view.pNext = nullptr;

			create_info_img_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
			create_info_img_view.image = img;
			create_info_img_view.format = format;
			create_info_img_view.subresourceRange.baseMipLevel = 0;
			create_info_img_view.subresourceRange.levelCount = 1;
			create_info_img_view.subresourceRange.baseArrayLayer = 0;
			create_info_img_view.subresourceRange.layerCount = 1;
			create_info_img_view.subresourceRange.aspectMask = aspect_flags;

			return create_info_img_view;
		}

		// Builder Classes

		void DescriptorLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type)
		{
			VkDescriptorSetLayoutBinding new_bind = {};
			new_bind.binding = binding;
			new_bind.descriptorCount = 1;
			new_bind.descriptorType = type;

			bindings.push_back(new_bind);
		}

		void DescriptorLayoutBuilder::Clear()
		{
			bindings.clear();
		}

		VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice device, VkShaderStageFlags shader_stages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0)
		{
			for (auto& b : bindings)
			{
				b.stageFlags |= shader_stages;
			}

			VkDescriptorSetLayoutCreateInfo create_info_descr = {};
			create_info_descr.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			create_info_descr.pNext = nullptr;

			create_info_descr.pBindings = bindings.data();
			create_info_descr.bindingCount = (uint32_t)bindings.size();
			create_info_descr.flags = flags;

			VkDescriptorSetLayout set;
			VkResult result = vkCreateDescriptorSetLayout(device, &create_info_descr, nullptr, &set);
			OB3D_VK_CHECK(result, "failed to create descriptor set layout!");

			return set;
		}

		void DescriptorAllocator::InitPool(VkDevice device, uint32_t max_sets, std::span<PoolSizeRatio> pool_ratios)
		{
			std::vector<VkDescriptorPoolSize> pool_sizes;
			for (PoolSizeRatio ratio : pool_ratios)
			{
				VkDescriptorPoolSize new_pool_size = {};
				new_pool_size.type = ratio.type;
				new_pool_size.descriptorCount = uint32_t(ratio.ratio * max_sets);
				pool_sizes.push_back(new_pool_size);
			}

			VkDescriptorPoolCreateInfo info_pool = {};
			info_pool.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			info_pool.flags = 0;
			info_pool.maxSets = max_sets;
			info_pool.poolSizeCount = (uint32_t)pool_sizes.size();
			info_pool.pPoolSizes = pool_sizes.data();

			vkCreateDescriptorPool(device, &info_pool, nullptr, &pool);
		}

		void DescriptorAllocator::ClearDescriptors(VkDevice device)
		{
			vkResetDescriptorPool(device, pool, 0);
		}

		void DescriptorAllocator::DestroyPool(VkDevice device)
		{
			vkDestroyDescriptorPool(device, pool, nullptr);
		}

		VkDescriptorSet DescriptorAllocator::Allocate(VkDevice device, VkDescriptorSetLayout layout)
		{
			VkDescriptorSetAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			alloc_info.pNext = nullptr;
			alloc_info.descriptorPool = pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = &layout;

			VkDescriptorSet ds;
			VkResult result = vkAllocateDescriptorSets(device, &alloc_info, &ds);
			OB3D_VK_CHECK(result, "failed to allocate descriptor sets!");

			return ds;
		}

	}
}