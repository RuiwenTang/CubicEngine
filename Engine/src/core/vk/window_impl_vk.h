#pragma once

#include <volk.h>

#include "core/window_impl.h"

namespace cubic {

class WindowImplVK : public WindowImpl {
 public:
  WindowImplVK(WindowProps props, GLFWwindow* nativeWindow, RenderSystem* renderSystem);

  ~WindowImplVK() override;

  bool Init() override;

 protected:
  void SwapWindowBuffer() override;

  void Terminate() override;

 private:
  VkInstance mInstance = nullptr;
  VkSurfaceKHR mSurface = nullptr;
};

}  // namespace cubic
