#include <cubic/core/application.h>

namespace cubic {

Application* g_app = nullptr;

Application* Application::Create(Application::Config config) {
  g_app = new Application(std::move(config));

  return g_app;
}

void Application::Terminate() {
  if (g_app == nullptr) {
    return;
  }

  delete g_app;
}

Application::Application(Config config) : mConfig(std::move(config)) {}

Application::~Application() {}

}  // namespace cubic
