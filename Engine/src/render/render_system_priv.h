#pragma once

#include <cubic/render/render_system.h>

#include <memory>

namespace cubic {

struct RenderSystemInfo {
  Backend backend = Backend::kInvalid;
};

class RenderSystemPriv : public RenderSystem {
 public:
  RenderSystemPriv() = default;

  ~RenderSystemPriv() override = default;

  virtual RenderSystemInfo* GetBackendInfo() = 0;

  static std::unique_ptr<RenderSystem> InitRenderSystem(bool enableDebug);

  std::shared_ptr<ShaderModule> CreateShaderModule(ShaderModuleDescriptor* desc) override;
};

}  // namespace cubic
