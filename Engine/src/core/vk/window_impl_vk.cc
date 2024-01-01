#include "core/vk/window_impl_vk.h"

namespace cubic {

WindowImplVK::WindowImplVK(WindowProps props, GLFWwindow* nativeWindow, RenderSystem* renderSystem)
    : WindowImpl(std::move(props), nativeWindow, renderSystem) {}

WindowImplVK::~WindowImplVK() {}

bool WindowImplVK::Init() { return true; }

void WindowImplVK::SwapWindowBuffer() {}

}  // namespace cubic