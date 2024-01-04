#pragma once

#include <memory>

#include "core/window_impl.h"

namespace cubic {

class WindowMTLPriv;

class WindowImplMTL : public WindowImpl {
 public:
  WindowImplMTL(WindowProps props, GLFWwindow* nativeWindow, RenderSystem* renderSystem);

  ~WindowImplMTL() override;

  bool Init() override;

  void Show(WindowClient* client) override;

 protected:
  void SwapWindowBuffer() override;

 private:
  std::unique_ptr<WindowMTLPriv> mPriv = {};
};

}  // namespace cubic
