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

  RenderSystemInfo* GetBackendInfo() override;

  CommandQueue* GetCommandQueue(QueueType type) override;

  std::shared_ptr<RenderPipeline> CreateRenderPipeline(RenderPipelineDescriptor* desc) override;

  bool Init();

 protected:
  std::shared_ptr<ShaderModule> CompileBackendShader(ShaderModuleDescriptor* desc,
                                                     const std::vector<uint32_t>& spv) override;

 private:
  std::unique_ptr<RenderSystemMTLPriv> mPriv = {};
};

}  // namespace cubic
