#include "render/vk/render_system_vk.h"

namespace cubic {

std::unique_ptr<RenderSystemVk> RenderSystemVk::Create() { return std::make_unique<RenderSystemVk>(); }

RenderSystemVk::~RenderSystemVk() {
  if (mInstance) {
    vkDestroyInstance(mInstance, nullptr);
  }
}

bool RenderSystemVk::Init() { return false; }

}  // namespace cubic
