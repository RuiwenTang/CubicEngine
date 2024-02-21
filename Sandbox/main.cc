#include <cubic/cubic.h>

#include <array>
#include <cmath>

using namespace cubic;

class SandboxClient : public WindowClient {
 public:
  SandboxClient() = default;
  ~SandboxClient() override = default;

  void OnWindowUpdate(const std::shared_ptr<Texture> &surfaceTexture, RenderSystem *renderSystem) override {
    InitMSAATextureIfNeed(renderSystem, surfaceTexture);

    InitPipelineIfNeed(renderSystem, surfaceTexture->GetDescriptor().format);

    InitBufferIfNeed(renderSystem);

    InitBindGroupIfNeed(renderSystem);

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

    render_pass->SetVertexBuffer(mBuffer, 0, 0);

    render_pass->SetIndexBuffer(mBuffer, sizeof(float) * 20);

    render_pass->SetBindGroup(0, mColorGroup);

    render_pass->DrawElements(6, 0);

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

    const char *vertex_code = R"(
        #version 450 core
        layout(location = 0) in vec2 aPos;

        void main() {
            gl_Position = vec4(aPos, 0.0, 1.0);
        }
    )";

    ShaderModuleDescriptor vs_desc{};
    vs_desc.stage = ShaderStage::kVertexShader;
    vs_desc.code = vertex_code;
    vs_desc.label = "basic vertex";

    auto vs_shader = renderSystem->CreateShaderModule(&vs_desc);

    const char *frag_code = R"(
      #version 450 core

      layout(set = 0, binding = 0) uniform UColorInfo {
        vec4 color;
      } uColor;

      layout(location = 0) out vec4 outColor;
      void main() {
        outColor = uColor.color;
      }
    )";

    ShaderModuleDescriptor fs_desc{};
    fs_desc.stage = ShaderStage::kFragmentShader;
    fs_desc.code = frag_code;
    fs_desc.label = "basic fragment";

    auto fs_shader = renderSystem->CreateShaderModule(&fs_desc);

    RenderPipelineDescriptor desc{};
    desc.vertexShader = vs_shader;
    desc.fragmentShader = fs_shader;

    desc.vertexBuffer.emplace_back(VertexBufferLayout{});
    desc.vertexBuffer[0].stride = 5 * sizeof(float);
    desc.vertexBuffer[0].attribute.emplace_back(VertexAttribute{VertexFormat::kFloat32x2, 0, 0});

    ColorTargetState color1{};
    color1.format = format;

    desc.colorCount = 1;
    desc.pColorTargets = &color1;
    desc.sampleCount = 4;

    auto group = renderSystem->CreateBindGroupLayout({GroupEntryInfo{
        0,
        EntryType::kUniformBuffer,
        ShaderStage::kFragmentShader,
    }});

    desc.layout = renderSystem->CreatePipelineLayout({group});

    mPipeline = renderSystem->CreateRenderPipeline(&desc);
  }

  void InitBufferIfNeed(RenderSystem *renderSystem) {
    if (mBuffer) {
      return;
    }

    std::vector<float> raw_vertex = {
        0.45f, -0.5f, 1.f, 0.f, 0.f,  // v1
        0.5f,  0.5f,  0.f, 1.f, 0.f,  // v2
        -0.5f, 0.5f,  0.f, 0.f, 1.f,  // v3
        -0.5f, -0.5f, 1.f, 1.f, 0.f,  // v4
    };

    std::vector<uint32_t> raw_index{
        0, 1, 2,  // triangle 1
        3, 0, 2,  // tirangle 2
    };

    BufferDescriptor stage_desc{};
    stage_desc.storageMode = BufferStorageMode::kCPUOnly;
    stage_desc.usage = BufferUsage::kBuffCopySrc;
    stage_desc.size = sizeof(float) * raw_vertex.size();
    stage_desc.data = raw_vertex.data();

    auto stage_buffer1 = renderSystem->CreateBuffer(&stage_desc);

    stage_desc.size = sizeof(uint32_t) * raw_index.size();
    stage_desc.data = raw_index.data();

    auto stage_buffer2 = renderSystem->CreateBuffer(&stage_desc);

    uint32_t offset = sizeof(float) * 20 + sizeof(uint32_t) * 6;

    if (offset <= renderSystem->GetMinBufferAlignment()) {
      offset = renderSystem->GetMinBufferAlignment();
    } else {
      auto align = renderSystem->GetMinBufferAlignment();
      auto details = offset % align;

      offset += (align - details);
    }

    std::array<float, 4> color{1.f, 0.1f, 0.5f, 1.0f};

    stage_desc.size = sizeof(float) * 4;
    stage_desc.data = color.data();

    auto stage_buffer3 = renderSystem->CreateBuffer(&stage_desc);

    if (stage_buffer1 == nullptr || stage_buffer2 == nullptr || stage_buffer3 == nullptr) {
      CUB_ERROR("[Sandbox] Faield create stage buffer");
      exit(-1);
    }

    uint32_t buffer_length = offset + stage_desc.size;

    BufferDescriptor desc{};
    desc.storageMode = BufferStorageMode::kGPUOnly;
    desc.usage =
        BufferUsage::kBuffCopyDst | BufferUsage::kBuffVertex | BufferUsage::kBuffIndex | BufferUsage::kBuffUniform;
    desc.size = buffer_length;

    mBuffer = renderSystem->CreateBuffer(&desc);

    if (mBuffer == nullptr) {
      CUB_ERROR("[Sandbox] Failed create gpu buffer");
      exit(-1);
    }

    auto queue = renderSystem->GetCommandQueue(cubic::QueueType::kGraphic);

    auto cmd = queue->GenCommandBuffer();

    cmd->CopyBufferToBuffer(mBuffer, 0, stage_buffer1, 0, sizeof(float) * raw_vertex.size());
    cmd->CopyBufferToBuffer(mBuffer, sizeof(float) * raw_vertex.size(), stage_buffer2, 0,
                            sizeof(uint32_t) * raw_index.size());
    cmd->CopyBufferToBuffer(mBuffer, offset, stage_buffer3, 0, sizeof(float) * color.size());

    queue->Submit(std::move(cmd));

    mUniformOffset = offset;
  }

  void InitBindGroupIfNeed(RenderSystem *renderSystem) {
    mColorGroup = renderSystem->CreateBindGroup(mPipeline->GetLayout()->GetGroup(0),
                                                {
                                                    GroupEntry{
                                                        0,
                                                        EntryType::kUniformBuffer,
                                                        BindResource(BufferView{mBuffer, mUniformOffset}),
                                                    },
                                                });
  }

 private:
  uint32_t mFrameNum = 0;
  uint32_t mUniformOffset = 0;
  std::shared_ptr<Texture> mMSAATarget = {};
  std::shared_ptr<RenderPipeline> mPipeline = {};
  std::shared_ptr<Buffer> mBuffer = {};
  std::shared_ptr<BindGroup> mColorGroup = {};
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
