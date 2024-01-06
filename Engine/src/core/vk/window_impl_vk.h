#pragma once

#include <volk.h>

#include "core/window_impl.h"

namespace cubic {

class VulkanDevice;

class WindowImplVK : public WindowImpl {
 public:
  WindowImplVK(WindowProps props, GLFWwindow* nativeWindow, RenderSystem* renderSystem);

  ~WindowImplVK() override;

  bool Init() override;

 protected:
  void SwapWindowBuffer() override;

  void Terminate() override;

  bool ChooseSurfaceFormat();

  bool CreateSwapchain();

 private:
  VkInstance mInstance = nullptr;
  VulkanDevice* mDevice = nullptr;
  VkSurfaceKHR mSurface = nullptr;
  VkSurfaceFormatKHR mSurfaceFormat = {};
};

}  // namespace cubic
