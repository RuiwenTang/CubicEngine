#pragma once

#include <cubic/render/render_system.h>

#include <memory>

namespace cubic {

class RenderSystemMTLPriv;

class RenderSystemMTL : public RenderSystem {
 public:
  static std::unique_ptr<RenderSystemMTL> Create();

  RenderSystemMTL();

  ~RenderSystemMTL() override;

  Backend GetBackend() const override { return Backend::kMetal; }

  bool Init();

 private:
  std::unique_ptr<RenderSystemMTLPriv> mPriv = {};
};

}  // namespace cubic
