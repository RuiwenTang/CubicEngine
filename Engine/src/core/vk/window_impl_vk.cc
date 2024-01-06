#include "core/vk/window_impl_vk.h"

#include "render/vk/render_system_vk.h"

namespace cubic {

WindowImplVK::WindowImplVK(WindowProps props, GLFWwindow* nativeWindow, RenderSystem* renderSystem)
    : WindowImpl(std::move(props), nativeWindow, renderSystem) {}

WindowImplVK::~WindowImplVK() {}

bool WindowImplVK::Init() {
  auto render_system = reinterpret_cast<RenderSystemVk*>(GetRenderSystem());

  if (render_system == nullptr) {
    return false;
  }

  auto vk_info = reinterpret_cast<RenderSystemInfoVK*>(render_system->GetBackendInfo());

  if (vk_info == nullptr) {
    return false;
  }

  mInstance = vk_info->instance;

  if (glfwCreateWindowSurface(mInstance, GetNativeWindow(), nullptr, &mSurface) != VK_SUCCESS) {
    return false;
  }

  return true;
}

void WindowImplVK::Terminate() {
  if (mSurface) {
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
  }
}

void WindowImplVK::SwapWindowBuffer() {}

}  // namespace cubic
