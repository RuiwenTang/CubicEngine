#pragma once

#include <cubic/core/window.h>

#include <memory>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace cubic {

class WindowImpl : public Window {
 public:
  static void InitPlatform();

  static void TerminatePlatform();

  static std::unique_ptr<Window> Create(WindowProps props, RenderSystem* renderSystem);

  WindowImpl(WindowProps props, GLFWwindow* nativeWindow, RenderSystem* renderSystem);

  ~WindowImpl() override;

  void Show(WindowClient* client) override;

  GLFWwindow* GetNativeWindow() const { return mNativeWindow; }

  RenderSystem* GetRenderSystem() const { return mRenderSystem; }

  virtual bool Init() = 0;

  virtual void Terminate() = 0;

 protected:
  virtual void SwapWindowBuffer() = 0;

 private:
  GLFWwindow* mNativeWindow;
  RenderSystem* mRenderSystem;
};

}  // namespace cubic
