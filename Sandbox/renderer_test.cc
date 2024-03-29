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
      mRenderer->SetNeedDepthBuffer(true);
    }

    if (mMesh == nullptr) {
      auto geom = Geometry::CreatePlane();
      auto box = Geometry::CreateBox(0.1f, 0.1f, 0.1f);
      auto material = std::make_shared<ColorMaterial>(1.f, 0.f, 0.f, 1.f);
      auto material2 = std::make_shared<ColorMaterial>(0.f, 1.f, 0.f, 1.f);

      mMesh = std::make_shared<Mesh>(box, material);

      mMesh2 = std::make_shared<Mesh>(geom, material2);

      mMesh2->GetTransform().Translate({0.2f, 0.f, 0.1f});
    }

    mMesh->GetTransform().Rotate({0.01f, 0.f, 0.f});

    float flash = std::abs(std::sin(mFrameNum / 120.f));
    mRenderer->SetClearColor(GPUColor{0.0, 0.0, flash, 1.0});

    mRenderer->Render(mMesh);

    mRenderer->Render(mMesh2);

    mRenderer->Flush(surfaceTexture);

    mFrameNum++;
  }

 private:
  uint32_t mFrameNum = 0;
  std::unique_ptr<Renderer> mRenderer;
  std::shared_ptr<Mesh> mMesh;
  std::shared_ptr<Mesh> mMesh2;
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
