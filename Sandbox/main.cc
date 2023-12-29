#include <cubic/cubic.h>

#include <iostream>

using namespace cubic;

int main(int argc, const char** argv) {
  Application::Config config;
  config.name = "Sandbox Application";

  auto app = Application::Create(config);

  std::cout << "app name: " << app->GetConfig().name << std::endl;

  Application::Terminate();

  return 0;
}