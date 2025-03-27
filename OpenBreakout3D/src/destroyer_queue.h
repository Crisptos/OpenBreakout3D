#pragma once
#include "util.h"

namespace OB3D
{
	enum class DestroyableVkType : uint8_t
	{
		DESTROYABLE_INSTANCE,
		DESTROYABLE_DEVICE,
		DESTROYABLE_SURFACE,
		DESTROYABLE_IMG_VIEW,
		DESTROYABLE_SWAPCHAIN,
		DESTROYABLE_SEMAPHORE,
		DESTROYABLE_FENCE,
		DESTROYABLE_COMMAND_POOL,
		DESTROYABLE_DBG_MESSENGER
	};

	struct Destroyable
	{
		union
		{
			VkInstance inst;
			VkDevice device;
			VkSurfaceKHR surface;
			VkImageView img_view;
			VkSwapchainKHR swapchain;
			VkSemaphore semaphore;
			VkFence fence;
			VkCommandPool cmd_pool;
			VkDebugUtilsMessengerEXT dbg_msg;
			uint64_t unknown;
		};

		DestroyableVkType type;
	};

	struct DestroyerQueue
	{
		std::vector<Destroyable> destroyables;

	public:
		void Push(const Destroyable& destroyable);
		void Flush();

	private:
		void DeletorHandle(Destroyable& destroyable);

	private:
		// References needed for deletion
		static VkInstance inst_handle;
		static VkDevice device_handle;
	};

}