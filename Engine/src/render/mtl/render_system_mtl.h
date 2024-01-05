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

  bool Init();

 private:
  std::unique_ptr<RenderSystemMTLPriv> mPriv = {};
};

}  // namespace cubic
