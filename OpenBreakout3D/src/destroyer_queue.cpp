#include "destroyer_queue.h"
#include "VkBootstrap.h"

namespace OB3D
{
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
				break;
			}
		
			case DestroyableVkType::DESTROYABLE_SWAPCHAIN:
			{
				break;
			}

			case DestroyableVkType::DESTROYABLE_SEMAPHORE:
			{
				break;
			}

			case DestroyableVkType::DESTROYABLE_FENCE:
			{
				break;
			}

			case DestroyableVkType::DESTROYABLE_COMMAND_POOL:
			{
				break;
			}

			case DestroyableVkType::DESTROYABLE_DBG_MESSENGER:
			{
				vkb::destroy_debug_utils_messenger(inst_handle, destroyable.dbg_msg);
				fmt::println("Dbg Messenger Function Destroyed Successfully");
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
