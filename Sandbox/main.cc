#include <cubic/cubic.h>

using namespace cubic;

int main(int argc, const char** argv) {
  Application::Config config;
  config.name = "Sandbox Application";

  if (!Application::Init(config)) {
    CUB_ERROR("Failed init Application!!");

    Application::Terminate();

    return -1;
  }

  auto app = Application::Get();

  CUB_INFO("App name : [ {} ]", app->GetConfig().name);

  WindowProps props{};

  props.title = "Hello Sandbox";
  props.width = 1000;
  props.height = 1000;
  props.resizeable = false;

  auto window = app->CreateWindow(props);

  if (window) {
    window->Show(nullptr);
  }

  Application::Terminate();

  return 0;
}
