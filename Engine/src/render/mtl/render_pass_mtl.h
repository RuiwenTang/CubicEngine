#pragma once

#import <Metal/Metal.h>
#include <cubic/render/render_pass.h>

namespace cubic {

class RenderPassMTL : public RenderPass {
 public:
  RenderPassMTL(id<MTLRenderCommandEncoder> encoder);

  ~RenderPassMTL() override;

  void BindPipeline(const std::shared_ptr<RenderPipeline> &pipeline) override;

  void SetVertexBuffer(const std::shared_ptr<Buffer> &buffer, uint32_t slot, uint64_t offset) override;

  void SetIndexBuffer(const std::shared_ptr<Buffer> &buffer, uint64_t offset) override;

  void Draw(uint32_t numVertex, uint32_t firstVertex) override;

  void DrawElements(uint32_t numIndices, uint32_t firstIndex) override;

  id<MTLRenderCommandEncoder> GetNativeEncoder() { return mEncoder; }

 private:
  id<MTLRenderCommandEncoder> mEncoder;

  std::shared_ptr<Buffer> mCurrIndex = {};
  uint64_t mCurrIndexOffset = 0;
};

}  // namespace cubic
