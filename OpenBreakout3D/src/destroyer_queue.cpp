#include "destroyer_queue.h"
#include "VkBootstrap.h"

namespace OB3D
{
	// Global instance and Device Handles for presceeding destroy calls
	VkInstance DestroyerQueue::inst_handle = nullptr;
	VkDevice DestroyerQueue::device_handle = nullptr;
	VmaAllocator DestroyerQueue::alloc_handle = nullptr;


	void DestroyerQueue::Push(const Destroyable& destroyable)
	{
		// Check for handles that need to be held onto for deletion of other handles
		if (destroyable.type == DestroyableVkType::DESTROYABLE_INSTANCE
			&& inst_handle == nullptr
			)
		{
			inst_handle = destroyable.inst;
		}

		if (destroyable.type == DestroyableVkType::DESTROYABLE_DEVICE
			&& device_handle == nullptr
			)
		{
			device_handle = destroyable.device;
		}

		if (destroyable.type == DestroyableVkType::DESTROYABLE_VMA
			&& alloc_handle == nullptr
			)
		{
			alloc_handle = destroyable.alloc;
		}

		destroyables.push_back(destroyable);
	}

	void DestroyerQueue::Flush()
	{
		while (!destroyables.empty())
		{
			DeletorHandle(destroyables.back());
			destroyables.pop_back();
		}
	}

	void DestroyerQueue::DeletorHandle(Destroyable& destroyable)
	{
		switch (destroyable.type)
		{
			case DestroyableVkType::DESTROYABLE_INSTANCE:
			{
				vkDestroyInstance(destroyable.inst, nullptr);
				inst_handle = nullptr;
				fmt::println("Instance Destroyed Successfully");
				break;
			}

			case DestroyableVkType::DESTROYABLE_DEVICE:
			{
				vkDestroyDevice(destroyable.device, nullptr);
				device_handle = nullptr;
				fmt::println("Logical Device Destroyed Successfully");
				break;
			}

			case DestroyableVkType::DESTROYABLE_SURFACE:
			{
				vkDestroySurfaceKHR(inst_handle, destroyable.surface, nullptr);
				fmt::println("SurfaceKHR Destroyed Successfully");
				break;
			}

			case DestroyableVkType::DESTROYABLE_IMG_VIEW:
			{
				vkDestroyImageView(device_handle, destroyable.img_view, nullptr);
				fmt::println("Img View Destroyed Successfully");
				break;
			}
			
			case DestroyableVkType::DESTROYABLE_IMG:
			{
				vmaDestroyImage(alloc_handle, destroyable.img, destroyable.allocation);
				fmt::println("Img Destroyed Successfully");
				break;
			}
		
			case DestroyableVkType::DESTROYABLE_SWAPCHAIN:
			{
				vkDestroySwapchainKHR(device_handle, destroyable.swapchain, nullptr);
				fmt::println("SwapchainKHR Destroyed Successfully");
				break;
			}

			case DestroyableVkType::DESTROYABLE_SEMAPHORE:
			{
				vkDestroySemaphore(device_handle, destroyable.semaphore, nullptr);
				break;
			}

			case DestroyableVkType::DESTROYABLE_FENCE:
			{
				vkDestroyFence(device_handle, destroyable.fence, nullptr);
				break;
			}

			case DestroyableVkType::DESTROYABLE_COMMAND_POOL:
			{
				vkDestroyCommandPool(device_handle, destroyable.cmd_pool, nullptr);
				break;
			}

			case DestroyableVkType::DESTROYABLE_DBG_MESSENGER:
			{
				vkb::destroy_debug_utils_messenger(inst_handle, destroyable.dbg_msg);
				fmt::println("Dbg Messenger Function Destroyed Successfully");
				break;
			}

			case DestroyableVkType::DESTROYABLE_VMA:
			{
				vmaDestroyAllocator(destroyable.alloc);
				alloc_handle = nullptr;
				fmt::println("VMA Destroyed Successfully");
				break;
			}

			default:
			{
				fmt::println("THIS MESSAGE SHOULD NOT BE PRINTING");
				std::abort();
				break;
			}

		}
	}
}
