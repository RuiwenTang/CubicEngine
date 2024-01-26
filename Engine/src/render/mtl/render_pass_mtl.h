#pragma once

#import <Metal/Metal.h>
#include <cubic/render/render_pass.h>

namespace cubic {

class RenderPassMTL : public RenderPass {
 public:
  RenderPassMTL(id<MTLRenderCommandEncoder> encoder);

  ~RenderPassMTL() override;

  void BindPipeline(const std::shared_ptr<RenderPipeline> &pipeline) override;

  void Draw(uint32_t numVertex, uint32_t firstVertex) override;

  id<MTLRenderCommandEncoder> GetNativeEncoder() { return mEncoder; }

 private:
  id<MTLRenderCommandEncoder> mEncoder;
};

}  // namespace cubic
