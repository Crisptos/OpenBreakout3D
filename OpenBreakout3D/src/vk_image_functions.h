#include "util.h"
#include "vk_constructors.h"

namespace OB3D
{
	namespace VkImageFunctions
	{
		void TransitionImage(VkCommandBuffer cmd, VkImage img, VkImageLayout current_layout, VkImageLayout new_layout);
	}
}