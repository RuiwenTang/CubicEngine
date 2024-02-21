#pragma once

#include <memory>

#include "render/render_system_priv.h"

namespace cubic {

class RenderSystemMTLPriv;

class RenderSystemMTL : public RenderSystemPriv {
 public:
  static std::unique_ptr<RenderSystemMTL> Create();

  RenderSystemMTL();

  ~RenderSystemMTL() override;

  Backend GetBackend() const override { return Backend::kMetal; }

  uint32_t GetMinBufferAlignment() const override;

  RenderSystemInfo* GetBackendInfo() override;

  CommandQueue* GetCommandQueue(QueueType type) override;

  std::shared_ptr<RenderPipeline> CreateRenderPipeline(RenderPipelineDescriptor* desc) override;

  std::shared_ptr<Texture> CreateTexture(TextureDescirptor* desc) override;

  std::shared_ptr<Buffer> CreateBuffer(BufferDescriptor* desc) override;

  std::shared_ptr<BindGroup> CreateBindGroup(const std::shared_ptr<BindGroupLayout>& layout,
                                             std::vector<GroupEntry> entries) override;

  bool Init();

 protected:
  std::shared_ptr<ShaderModule> CompileBackendShader(ShaderModuleDescriptor* desc,
                                                     const std::vector<uint32_t>& spv) override;

 private:
  std::unique_ptr<RenderSystemMTLPriv> mPriv = {};
};

}  // namespace cubic
