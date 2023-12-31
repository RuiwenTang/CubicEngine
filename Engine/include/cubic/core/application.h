#pragma once

#include <cubic/platform.h>
#include <cubic/render/render_system.h>

#include <memory>
#include <string>

namespace cubic {

class CUBIC_API Application {
 public:
  struct Config {
    std::string name = "Cubic Application";
    std::string workingDirectory;

    bool debugModle = true;
  };

  static bool Init(Application::Config config);

  static Application* Get();

  static void Terminate();

  ~Application();

  const Config& GetConfig() const { return mConfig; }

 private:
  Application(Config config);

  bool InitInternal();

 private:
  Config mConfig;

  std::unique_ptr<RenderSystem> mRenderSystem = {};
};

}  // namespace cubic
