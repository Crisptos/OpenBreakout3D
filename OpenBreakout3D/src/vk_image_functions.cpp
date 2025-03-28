#include "vk_image_functions.h"

namespace OB3D
{
	namespace VkImageFunctions
	{
		void TransitionImage(VkCommandBuffer cmd, VkImage img, VkImageLayout current_layout, VkImageLayout new_layout)
		{
			VkImageMemoryBarrier2 img_barrier = {};
			img_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
			img_barrier.pNext = nullptr;

			img_barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
			img_barrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
			img_barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
			img_barrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

			img_barrier.oldLayout = current_layout;
			img_barrier.newLayout = new_layout;

			VkImageAspectFlags aspect_mask = (new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
			img_barrier.subresourceRange = VkConstructors::ImageSubresourceRange(aspect_mask);
			img_barrier.image = img;

			VkDependencyInfo dep_info = {};
			dep_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
			dep_info.pNext = nullptr;
			dep_info.imageMemoryBarrierCount = 1;
			dep_info.pImageMemoryBarriers = &img_barrier;
			vkCmdPipelineBarrier2(cmd, &dep_info);
		}

		void CopyImageToImage(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D src_ext, VkExtent2D dst_ext)
		{
			VkImageBlit2 blit_region = {};
			blit_region.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
			blit_region.pNext = nullptr;

			blit_region.srcOffsets[1].x = src_ext.width;
			blit_region.srcOffsets[1].y = src_ext.height;
			blit_region.srcOffsets[1].z = 1;

			blit_region.dstOffsets[1].x = dst_ext.width;
			blit_region.dstOffsets[1].y = dst_ext.height;
			blit_region.dstOffsets[1].z = 1;

			blit_region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit_region.srcSubresource.baseArrayLayer = 0;
			blit_region.srcSubresource.layerCount = 1;
			blit_region.srcSubresource.mipLevel = 0;

			blit_region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit_region.dstSubresource.baseArrayLayer = 0;
			blit_region.dstSubresource.layerCount = 1;
			blit_region.dstSubresource.mipLevel = 0;

			VkBlitImageInfo2 blit_img_info = {};
			blit_img_info.sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
			blit_img_info.pNext = nullptr;
			blit_img_info.dstImage = destination;
			blit_img_info.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			blit_img_info.srcImage = source;
			blit_img_info.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			blit_img_info.filter = VK_FILTER_LINEAR;
			blit_img_info.regionCount = 1;
			blit_img_info.pRegions = &blit_region;

			vkCmdBlitImage2(cmd, &blit_img_info);
		}
	}
}