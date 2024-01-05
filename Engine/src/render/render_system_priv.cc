#include "render/render_system_priv.h"

#include <cubic/core/log.h>

#if defined(CUBIC_PLATFORM_WINDOWS)

#include "render/vk/render_system_vk.h"

#elif defined(CUBIC_PLATFORM_MACOS)

#include "render/mtl/render_system_mtl.h"

#endif

namespace cubic {

std::unique_ptr<RenderSystem> RenderSystemPriv::InitRenderSystem(bool enableDebug) {
#if defined(CUBIC_PLATFORM_WINDOWS)

  CUB_INFO("Init Vulkan backend...");

  auto render_system = RenderSystemVk::Create();

  if (!render_system->Init(enableDebug)) {
    CUB_ERROR("Failed init vulkan backend context !!");

    return std::unique_ptr<RenderSystem>();
  }

  return render_system;
#elif defined(CUBIC_PLATFORM_MACOS)

  CUB_INFO("Init Metal backend...");

  auto render_system = RenderSystemMTL::Create();

  if (!render_system->Init()) {
    CUB_ERROR("Failed init Metal backend context !!");

    return std::unique_ptr<RenderSystem>();
  }

  return render_system;
#endif

  return std::unique_ptr<RenderSystem>();
}

}  // namespace cubic
