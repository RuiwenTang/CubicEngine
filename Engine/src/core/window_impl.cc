#include "core/window_impl.h"

#include <cubic/platform.h>

#if defined(CUBIC_PLATFORM_WINDOWS)
#include "core/vk/window_impl_vk.h"
#elif defined(CUBIC_PLATFORM_MACOS)
#include "core/mtl/window_impl_mtl.h"
#endif

namespace cubic {

void WindowImpl::InitPlatform() { glfwInit(); }

void WindowImpl::TerminatePlatform() { glfwTerminate(); }

std::unique_ptr<Window> WindowImpl::Create(WindowProps props, RenderSystem* renderSystem) {
#if defined(CUBIC_PLATFORM_WINDOWS)

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, props.resizeable ? GLFW_TRUE : GLFW_FALSE);

  auto native_window = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);

  if (native_window == nullptr) {
    return std::unique_ptr<Window>();
  }

  auto window = std::make_unique<WindowImplVK>(std::move(props), native_window, renderSystem);

  if (!window->Init()) {
    return std::unique_ptr<Window>();
  }

  return window;
#elif defined(CUBIC_PLATFORM_MACOS)
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, props.resizeable ? GLFW_TRUE : GLFW_FALSE);

  auto native_window = glfwCreateWindow(props.width, props.height, props.title.c_str(), nullptr, nullptr);

  if (native_window == nullptr) {
    return std::unique_ptr<Window>();
  }

  auto window = std::make_unique<WindowImplMTL>(std::move(props), native_window, renderSystem);

  if (!window->Init()) {
    return std::unique_ptr<Window>();
  }

  return window;
#endif

  return std::unique_ptr<Window>();
}

WindowImpl::WindowImpl(WindowProps props, GLFWwindow* nativeWindow, RenderSystem* renderSystem)
    : Window(std::move(props)), mNativeWindow(nativeWindow), mRenderSystem(renderSystem) {}

WindowImpl::~WindowImpl() {
  if (mNativeWindow) {
    glfwDestroyWindow(mNativeWindow);
  }
}

void WindowImpl::Show(WindowClient* client) {
  while (!glfwWindowShouldClose(mNativeWindow)) {
    glfwPollEvents();

    if (client) {
      client->OnWindowUpdate(this, mRenderSystem);
    }

    SwapWindowBuffer();
  }

  Terminate();
}

}  // namespace cubic
