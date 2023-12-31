#include "render/render_system_priv.h"

#include <cubic/core/log.h>

#if defined(CUBIC_PLATFORM_WINDOWS)

#include "render/vk/render_system_vk.h"

#endif

namespace cubic {

std::unique_ptr<RenderSystem> InitRenderSystem(bool enableDebug) {
#if defined(CUBIC_PLATFORM_WINDOWS)

  CUB_INFO("Init Vulkan backend...");

  auto render_system = RenderSystemVk::Create();

  if (!render_system->Init(enableDebug)) {
    CUB_ERROR("Failed init vulkan backend context !!");

    return std::unique_ptr<RenderSystem>();
  }

  return render_system;
#endif

  return std::unique_ptr<RenderSystem>();
}

}  // namespace cubic
