#pragma once

#include <volk.h>

#include "render/render_system_priv.h"

namespace cubic {

class VulkanDevice;

struct RenderSystemInfoVK : public RenderSystemInfo {
  VulkanDevice* device = nullptr;
};

}  // namespace cubic
