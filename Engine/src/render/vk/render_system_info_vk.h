#pragma once

#include <volk.h>

#include "render/render_system_priv.h"

namespace cubic {

class VulkanDevice;

struct RenderSystemInfoVK : public RenderSystemInfo {
  VkInstance instance = VK_NULL_HANDLE;
  VulkanDevice* device = nullptr;
};

}  // namespace cubic
