#pragma once

#include "core/window_impl.h"

namespace cubic {

class WindowImplVK : public WindowImpl {
 public:
  WindowImplVK(WindowProps props, GLFWwindow* nativeWindow, RenderSystem* renderSystem);

  ~WindowImplVK() override;

  bool Init() override;

 protected:
  void SwapWindowBuffer() override;
};

}  // namespace cubic
