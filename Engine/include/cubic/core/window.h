#pragma once

#include <memory>
#include <string>

namespace cubic {

struct WindowProps {
  std::string title = "";

  uint32_t width = 800;
  uint32_t height = 800;

  bool resizeable = false;
};

class RenderSystem;
class Texture;

class WindowClient {
 public:
  virtual ~WindowClient() = default;

  virtual void OnWindowUpdate(const std::shared_ptr<Texture>& surfaceTexture, RenderSystem* renderSystem) = 0;
};

class Window {
 public:
  Window(WindowProps props) : mProps(std::move(props)) {}

  virtual ~Window() = default;

  uint32_t GetWidth() const { return mProps.width; }
  uint32_t GetHeight() const { return mProps.height; }
  const std::string& GetTitle() const { return mProps.title; }

  virtual void Show(WindowClient* client) = 0;

 protected:
  const WindowProps& GetProps() const { return mProps; }

 private:
  WindowProps mProps;
};

}  // namespace cubic
