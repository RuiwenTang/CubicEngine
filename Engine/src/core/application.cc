#include <cubic/core/application.h>
#include <cubic/core/log.h>

namespace cubic {

Application* g_app = nullptr;

bool Application::Init(Application::Config config) {
  if (g_app) {
    CUB_ERROR("Application already initialied !!");
    return false;
  } else {
    g_app = new Application(std::move(config));
    return true;
  }
}

Application* Application::Get() { return g_app; }

void Application::Terminate() {
  if (g_app == nullptr) {
    return;
  }

  delete g_app;
}

Application::Application(Config config) : mConfig(std::move(config)) {}

Application::~Application() {}

}  // namespace cubic
