#include "core/mtl/window_impl_mtl.h"
#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

namespace cubic {

class WindowMTLPriv {
 public:
  WindowMTLPriv(GLFWwindow* nativeWindow) : mNativeWindow(nativeWindow) {}

  ~WindowMTLPriv() = default;

  bool Init() {
    mLayer = [CAMetalLayer new];

    if (mLayer == nil) {
      return false;
    }

    NSWindow* ns_window = glfwGetCocoaWindow(mNativeWindow);

    if (ns_window == nil) {
      return false;
    }

    ns_window.contentView.layer = mLayer;
    ns_window.contentView.wantsLayer = YES;

    return true;
  }

 private:
  GLFWwindow* mNativeWindow;
  CAMetalLayer* mLayer = nil;
};

WindowImplMTL::WindowImplMTL(WindowProps props, GLFWwindow* nativeWindow, RenderSystem* renderSystem)
    : WindowImpl(std::move(props), nativeWindow, renderSystem) {}

WindowImplMTL::~WindowImplMTL() {}

bool WindowImplMTL::Init() {
  mPriv = std::make_unique<WindowMTLPriv>(GetNativeWindow());

  return mPriv->Init();
}

void WindowImplMTL::Show(WindowClient* client) {
  while (!glfwWindowShouldClose(GetNativeWindow())) {
    glfwPollEvents();

    @autoreleasepool {
      if (client) {
        client->OnWindowUpdate(this, GetRenderSystem());
      }

      SwapWindowBuffer();
    }
  }
}

void WindowImplMTL::SwapWindowBuffer() {}

}
