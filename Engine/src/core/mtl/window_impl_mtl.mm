#include "core/mtl/window_impl_mtl.h"
#include <GLFW/glfw3.h>

namespace cubic {

class WindowMTLPriv {
 public:
  WindowMTLPriv(GLFWwindow* nativeWindow) : mNativeWindow(nativeWindow) {}

  ~WindowMTLPriv() = default;

  bool Init() { return true; }

 private:
  GLFWwindow* mNativeWindow;
};

WindowImplMTL::WindowImplMTL(WindowProps props, GLFWwindow* nativeWindow, RenderSystem* renderSystem)
    : WindowImpl(std::move(props), nativeWindow, renderSystem) {}

WindowImplMTL::~WindowImplMTL() {}

bool WindowImplMTL::Init() {
  mPriv = std::make_unique<WindowMTLPriv>(GetNativeWindow());

  return mPriv->Init();
}

void WindowImplMTL::SwapWindowBuffer() {}

}
