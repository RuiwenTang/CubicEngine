#pragma once

#include <cubic/platform.h>

#include <string>

namespace cubic {

class CUBIC_API Application {
 public:
  struct Config {
    std::string name = "Cubic Application";
    std::string workingDirectory;
  };

  static bool Init(Application::Config config);

  static Application* Get();

  static void Terminate();

  ~Application();

  const Config& GetConfig() const { return mConfig; }

 private:
  Application(Config config);

 private:
  Config mConfig;
};

}  // namespace cubic
