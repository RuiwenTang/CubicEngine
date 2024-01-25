#include <cubic/cubic.h>

#include <cmath>

using namespace cubic;

class SandboxClient : public WindowClient {
 public:
  SandboxClient() = default;
  ~SandboxClient() override = default;

  void OnWindowUpdate(const std::shared_ptr<Texture> &surfaceTexture, RenderSystem *renderSystem) override {
    InitPipelineIfNeed(renderSystem);

    auto queue = renderSystem->GetCommandQueue(QueueType::kGraphic);

    auto cmd = queue->GenCommandBuffer();

    RenderPassDescriptor desc{};

    ColorAttachment color_attachment{};

    float flash = std::abs(std::sin(mFrameNum / 120.f));

    color_attachment.clearValue = GPUColor{0.0, 0.0, flash, 1.0};
    color_attachment.loadOp = LoadOp::kClear;
    color_attachment.storeOp = StoreOp::kStore;
    color_attachment.target = surfaceTexture;

    desc.colorAttachmentCount = 1;
    desc.pColorAttachments = &color_attachment;

    auto render_pass = cmd->BeginRenderPass(desc);

    cmd->EndRenderPass(std::move(render_pass));

    queue->Submit(std::move(cmd));

    mFrameNum++;
  }

  void InitPipelineIfNeed(RenderSystem *renderSystem) {
    if (mVertexShader && mFragmentShader) {
      return;
    }

    const char *vertex_code = R"(#version 450 core
        vec2  positions[3] = vec2[](
            vec2(0.0, -0.5),
            vec2(0.5, 0.5),
            vec2(-0.5, 0.5)
        );
        void main() {
            gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
        }
    )";

    ShaderModuleDescriptor vs_desc{};
    vs_desc.stage = ShaderStage::kVertex;
    vs_desc.code = vertex_code;
    vs_desc.label = "basic vertex";

    mVertexShader = renderSystem->CreateShaderModule(&vs_desc);

    const char *frag_code = R"(#version 450 core
      layout(location = 0) out vec4 outColor;
      void main() {
        outColor = vec4(1.0, 0.0, 0.0, 1.0);
      }
    )";

    ShaderModuleDescriptor fs_desc{};
    fs_desc.stage = ShaderStage::kFragment;
    fs_desc.code = frag_code;
    fs_desc.label = "basic fragment";

    mFragmentShader = renderSystem->CreateShaderModule(&fs_desc);
  }

 private:
  uint32_t mFrameNum = 0;
  std::shared_ptr<ShaderModule> mVertexShader = {};
  std::shared_ptr<ShaderModule> mFragmentShader = {};
};

int main(int argc, const char **argv) {
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

  SandboxClient client{};

  if (window) {
    window->Show(&client);
  }

  Application::Terminate();

  return 0;
}
