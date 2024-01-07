#include "core/vk/window_impl_vk.h"

#include <cubic/core/log.h>

#include <vector>

#include "render/vk/render_system_vk.h"
#include "render/vk/vulkan_device.h"
#include "render/vk/command_queue_vk.h"

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

  if (!CreateSemaphore()) {
    return false;
  }

  return true;
}

void WindowImplVK::Terminate() {
  if (mRenderComplete) {
    vkDestroySemaphore(mDevice->GetLogicalDevice(), mRenderComplete, nullptr);
  }

  if (mPresentComplete) {
    vkDestroySemaphore(mDevice->GetLogicalDevice(), mPresentComplete, nullptr);
  }

  mSwapchain.reset();

  if (mSurface) {
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
  }
}

std::shared_ptr<Texture> WindowImplVK::AcquireTexture() {
  auto state = mSwapchain->AcquireNextFrame(mPresentComplete, VK_NULL_HANDLE);

  if (state.state != VK_SUCCESS) {
    return std::shared_ptr<Texture>();
  }

  mCurrentFrame = state.texture;

  mDevice->GetGraphicQueue()->ResetPool();
  mDevice->GetComputeQueue()->ResetPool();
  mDevice->GetTransferQueue()->ResetPool();

  return state.texture;
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

bool WindowImplVK::CreateSwapchain() {
  mSwapchain = std::make_unique<Swapchain>(mInstance, mSurface, mDevice);

  int32_t w, h;

  glfwGetFramebufferSize(GetNativeWindow(), &w, &h);

  return mSwapchain->Resize(w, h, mSurfaceFormat);
}

bool WindowImplVK::CreateSemaphore() {
  VkSemaphoreCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  if (vkCreateSemaphore(mDevice->GetLogicalDevice(), &create_info, nullptr, &mRenderComplete) != VK_SUCCESS) {
    return false;
  }

  if (vkCreateSemaphore(mDevice->GetLogicalDevice(), &create_info, nullptr, &mPresentComplete) != VK_SUCCESS) {
    return false;
  }

  return true;
}

void WindowImplVK::SwapWindowBuffer() {
  if (mCurrentFrame == nullptr) {
    return;
  }

  auto vk_texture = dynamic_cast<TextureVK*>(mCurrentFrame.get());

  mSwapchain->SubmitFrame(mDevice->GetTransferQueue(), std::move(mCurrentFrame), mPresentComplete);
}

}  // namespace cubic
