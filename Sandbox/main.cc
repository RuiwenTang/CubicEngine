#include <cubic/cubic.h>

using namespace cubic;

int main(int argc, const char** argv) {
  Application::Config config;
  config.name = "Sandbox Application";

  auto app = Application::Create(config);

  CUB_INFO("App name : [ {} ]", app->GetConfig().name);

  Application::Terminate();

  return 0;
}
