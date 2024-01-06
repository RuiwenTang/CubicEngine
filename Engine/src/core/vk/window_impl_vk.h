#pragma once

#include <volk.h>

#include "core/vk/swap_chain.h"
#include "core/window_impl.h"

namespace cubic {

class VulkanDevice;

class WindowImplVK : public WindowImpl {
 public:
  WindowImplVK(WindowProps props, GLFWwindow* nativeWindow, RenderSystem* renderSystem);

  ~WindowImplVK() override;

  bool Init() override;

  void Terminate() override;

 protected:
  void SwapWindowBuffer() override;

  bool ChooseSurfaceFormat();

  bool CreateSwapchain();

 private:
  VkInstance mInstance = nullptr;
  VulkanDevice* mDevice = nullptr;
  VkSurfaceKHR mSurface = nullptr;
  VkSurfaceFormatKHR mSurfaceFormat = {};

  std::unique_ptr<Swapchain> mSwapchain = {};
};

}  // namespace cubic
