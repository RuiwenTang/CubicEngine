#include <cubic/core/application.h>
#include <cubic/core/log.h>

#include "core/window_impl.h"
#include "render/render_system_priv.h"

namespace cubic {

Application* g_app = nullptr;

bool Application::Init(Application::Config config) {
  if (g_app) {
    CUB_ERROR("Application already initialied !!");
    return false;
  } else {
    g_app = new Application(std::move(config));

    if (!g_app->InitInternal()) {
      delete g_app;

      g_app = nullptr;

      return false;
    }

    return true;
  }
}

Application* Application::Get() { return g_app; }

void Application::Terminate() {
  if (g_app == nullptr) {
    return;
  }

  WindowImpl::TerminatePlatform();

  delete g_app;
}

std::unique_ptr<Window> Application::CreateWindow(WindowProps props) {
  return WindowImpl::Create(std::move(props), mRenderSystem.get());
}

Application::Application(Config config) : mConfig(std::move(config)) {}

bool Application::InitInternal() {
  mRenderSystem = InitRenderSystem(mConfig.debugModle);

  if (!mRenderSystem) {
    CUB_ERROR("Failed init RenderSystem for CubicEngine !!");
    return false;
  }

  WindowImpl::InitPlatform();

  return true;
}

Application::~Application() {}

}  // namespace cubic
