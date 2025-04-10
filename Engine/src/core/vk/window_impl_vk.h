#pragma once

#include <volk.h>

#include "core/vk/swap_chain.h"
#include "core/window_impl.h"
#include "render/vk/texture_vk.h"

namespace cubic {

class VulkanDevice;

class WindowImplVK : public WindowImpl {
 public:
  WindowImplVK(WindowProps props, GLFWwindow* nativeWindow, RenderSystem* renderSystem);

  ~WindowImplVK() override;

  bool Init() override;

  void Terminate() override;

 protected:
  std::shared_ptr<Texture> AcquireTexture() override;

  void SwapWindowBuffer() override;

  bool ChooseSurfaceFormat();

  bool CreateSwapchain();

  bool CreateVkSemaphore();

 private:
  VkInstance mInstance = nullptr;
  VulkanDevice* mDevice = nullptr;
  VkSurfaceKHR mSurface = nullptr;
  VkSurfaceFormatKHR mSurfaceFormat = {};

  std::unique_ptr<Swapchain> mSwapchain = {};

  VkSemaphore mPresentComplete = VK_NULL_HANDLE;
  VkSemaphore mRenderComplete = VK_NULL_HANDLE;

  VkFence mSwapchainFence = VK_NULL_HANDLE;

  std::shared_ptr<TextureVK> mCurrentFrame = {};
};

}  // namespace cubic
