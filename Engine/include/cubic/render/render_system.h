#pragma once

#include <cubic/platform.h>
#include <cubic/render/bind_group.h>
#include <cubic/render/buffer.h>
#include <cubic/render/command_queue.h>
#include <cubic/render/pipeline_layout.h>
#include <cubic/render/render_pipeline.h>
#include <cubic/render/shader_module.h>
#include <cubic/render/texture.h>

#include <memory>

namespace cubic {

enum class Backend {
  /// Used on Windows, Linux, and Android
  kVulkan,
  /// Used on MacOS and iOS
  kMetal,

  kInvalid,
};

class CUBIC_API RenderSystem {
 public:
  virtual ~RenderSystem() = default;

  virtual Backend GetBackend() const = 0;

  virtual CommandQueue* GetCommandQueue(QueueType type) = 0;

  virtual uint32_t GetMinBufferAlignment() const = 0;

  virtual std::shared_ptr<ShaderModule> CreateShaderModule(ShaderModuleDescriptor* desc) = 0;

  virtual std::shared_ptr<RenderPipeline> CreateRenderPipeline(RenderPipelineDescriptor* desc) = 0;

  virtual std::shared_ptr<Texture> CreateTexture(TextureDescirptor* desc) = 0;

  virtual std::shared_ptr<Buffer> CreateBuffer(BufferDescriptor* desc) = 0;

  virtual std::shared_ptr<BindGroupLayout> CreateBindGroupLayout(std::vector<GroupEntryInfo> entries) = 0;

  virtual std::shared_ptr<PipelineLayout> CreatePipelineLayout(
      std::vector<std::shared_ptr<BindGroupLayout>> groups) = 0;
};

}  // namespace cubic
