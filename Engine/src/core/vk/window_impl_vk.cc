#include "core/vk/window_impl_vk.h"

#include <cubic/core/log.h>

#include <vector>

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
  mDevice = vk_info->device;

  if (glfwCreateWindowSurface(mInstance, GetNativeWindow(), nullptr, &mSurface) != VK_SUCCESS) {
    CUB_ERROR("Failed create vulkan surface !!");
    return false;
  }

  if (!ChooseSurfaceFormat()) {
    CUB_ERROR("Failed query supported surface format !!");
    return false;
  }

  if (!CreateSwapchain()) {
    CUB_ERROR("Failed create swapchain !!");
    return false;
  }

  return true;
}

void WindowImplVK::Terminate() {
  if (mSurface) {
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
  }
}

bool WindowImplVK::ChooseSurfaceFormat() {
  uint32_t format_count = 0;

  vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->GetPhysicalDevice(), mSurface, &format_count, nullptr);

  if (format_count == 0) {
    return false;
  }

  std::vector<VkSurfaceFormatKHR> supported_formats(format_count);
  vkGetPhysicalDeviceSurfaceFormatsKHR(mDevice->GetPhysicalDevice(), mSurface, &format_count, supported_formats.data());

  mSurfaceFormat = supported_formats[0];

  std::vector<VkFormat> preferred_formats = {
      VK_FORMAT_B8G8R8A8_UNORM,
      VK_FORMAT_R8G8B8A8_UNORM,
      VK_FORMAT_A8B8G8R8_UNORM_PACK32,
  };

  for (const auto& fmt : supported_formats) {
    if (std::find(preferred_formats.begin(), preferred_formats.end(), fmt.format) != preferred_formats.end()) {
      mSurfaceFormat = fmt;
      break;
    }
  }

  return true;
}

bool WindowImplVK::CreateSwapchain() { return false; }

void WindowImplVK::SwapWindowBuffer() {}

}  // namespace cubic
