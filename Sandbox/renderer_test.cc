#include <cubic/cubic.h>

#include <array>
#include <cmath>

using namespace cubic;

class SandboxClient : public WindowClient {
 public:
  SandboxClient() = default;
  ~SandboxClient() override = default;

  void OnWindowUpdate(const std::shared_ptr<Texture> &surfaceTexture, RenderSystem *renderSystem) override {
    if (mRenderer == nullptr) {
      mRenderer = std::make_unique<Renderer>(renderSystem);

      mRenderer->SetAntialiasing(true);
      mRenderer->SetNeedDepthBuffer(false);
    }

    float flash = std::abs(std::sin(mFrameNum / 120.f));
    mRenderer->SetClearColor(GPUColor{0.0, 0.0, flash, 1.0});

    mRenderer->Flush(surfaceTexture);

    mFrameNum++;
  }

 private:
  uint32_t mFrameNum = 0;
  std::unique_ptr<Renderer> mRenderer;
};

int main(int argc, const char **argv) {
  Application::Config config;
  config.name = "Renderer Test";

  if (!Application::Init(config)) {
    CUB_ERROR("Failed init Application!!");

    Application::Terminate();

    return -1;
  }

  auto app = Application::Get();

  CUB_INFO("App name : [ {} ]", app->GetConfig().name);

  WindowProps props{};

  props.title = "Hello Renderer";
  props.width = 1000;
  props.height = 1000;
  props.resizeable = false;

  auto window = app->CreateWindow(props);

  {
    SandboxClient client{};

    if (window) {
      window->Show(&client);
    }
  }

  Application::Terminate();

  return 0;
}