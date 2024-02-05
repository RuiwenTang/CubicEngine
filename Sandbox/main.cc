#include <cubic/cubic.h>

#include <cmath>

using namespace cubic;

class SandboxClient : public WindowClient {
 public:
  SandboxClient() = default;
  ~SandboxClient() override = default;

  void OnWindowUpdate(const std::shared_ptr<Texture> &surfaceTexture, RenderSystem *renderSystem) override {
    InitMSAATextureIfNeed(renderSystem, surfaceTexture);

    InitPipelineIfNeed(renderSystem, surfaceTexture->GetDescriptor().format);

    auto queue = renderSystem->GetCommandQueue(QueueType::kGraphic);

    auto cmd = queue->GenCommandBuffer();

    RenderPassDescriptor desc{};

    ColorAttachment color_attachment{};

    float flash = std::abs(std::sin(mFrameNum / 120.f));

    color_attachment.clearValue = GPUColor{0.0, 0.0, flash, 1.0};
    color_attachment.loadOp = LoadOp::kClear;
    color_attachment.storeOp = StoreOp::kMSAAResolve;
    color_attachment.target = mMSAATarget;
    color_attachment.resolveTarget = surfaceTexture;

    desc.colorAttachmentCount = 1;
    desc.pColorAttachments = &color_attachment;

    auto render_pass = cmd->BeginRenderPass(desc);

    render_pass->BindPipeline(mPipeline);

    render_pass->Draw(3, 0);

    cmd->EndRenderPass(std::move(render_pass));

    queue->Submit(std::move(cmd));

    mFrameNum++;
  }

  void InitMSAATextureIfNeed(RenderSystem *renderSystem, const std::shared_ptr<Texture> &target) {
    if (mMSAATarget) {
      auto const &msaaDesc = mMSAATarget->GetDescriptor();
      auto const &targetDesc = target->GetDescriptor();

      if (msaaDesc.width == targetDesc.width && msaaDesc.height == targetDesc.height &&
          msaaDesc.format == targetDesc.format) {
        return;
      }
    }

    TextureDescirptor desc = target->GetDescriptor();

    desc.sample_count = 4;
    desc.usage = TextureUsage::kRenderTarget;

    mMSAATarget = renderSystem->CreateTexture(&desc);
  }

  void InitPipelineIfNeed(RenderSystem *renderSystem, TextureFormat format) {
    if (mPipeline) {
      return;
    }

    const char *vertex_code = R"(#version 450 core
        vec2  positions[3] = vec2[](
            vec2(0.45, -0.5),
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

    auto vs_shader = renderSystem->CreateShaderModule(&vs_desc);

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

    auto fs_shader = renderSystem->CreateShaderModule(&fs_desc);

    RenderPipelineDescriptor desc{};
    desc.vertexShader = vs_shader;
    desc.fragmentShader = fs_shader;

    ColorTargetState color1{};
    color1.format = format;

    desc.colorCount = 1;
    desc.pColorTargets = &color1;
    desc.sampleCount = 4;

    mPipeline = renderSystem->CreateRenderPipeline(&desc);
  }

  void InitBufferIfNeed(RenderSystem *renderSystem) {
    if (mBuffer) {
      return;
    }

    std::vector<float> raw_vertex = {0.45f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f};

    BufferDescriptor stage_desc{};
    stage_desc.storageMode = BufferStorageMode::kCPUOnly;
    stage_desc.usage = BufferUsage::kBuffCopySrc;
    stage_desc.size = sizeof(float) * raw_vertex.size();
    stage_desc.data = raw_vertex.data();

    auto stage_buffer = renderSystem->CreateBuffer(&stage_desc);

    if (stage_buffer == nullptr) {
      CUB_ERROR("[Sandbox] Faield create stage buffer");
      exit(-1);
    }

    BufferDescriptor desc{};
    desc.storageMode = BufferStorageMode::kGPUOnly;
    desc.usage = BufferUsage::kBuffCopyDst | BufferUsage::kBuffVertex;
    desc.size = stage_desc.size;

    mBuffer = renderSystem->CreateBuffer(&desc);

    if (mBuffer == nullptr) {
      CUB_ERROR("[Sandbox] Failed create gpu buffer");
      exit(-1);
    }

    auto queue = renderSystem->GetCommandQueue(cubic::QueueType::kGraphic);

    auto cmd = queue->GenCommandBuffer();

    cmd->CopyBufferToBuffer(mBuffer, 0, stage_buffer, 0, stage_desc.size);

    queue->Submit(std::move(cmd));
  }

 private:
  uint32_t mFrameNum = 0;
  std::shared_ptr<Texture> mMSAATarget = {};
  std::shared_ptr<RenderPipeline> mPipeline = {};
  std::shared_ptr<Buffer> mBuffer = {};
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

  {
    SandboxClient client{};

    if (window) {
      window->Show(&client);
    }
  }

  Application::Terminate();

  return 0;
}
