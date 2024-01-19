#pragma once

#import <Metal/Metal.h>
#include <cubic/render/render_pass.h>

namespace cubic {

class RenderPassMTL : public RenderPass {
 public:
  RenderPassMTL(id<MTLRenderCommandEncoder> encoder);

  ~RenderPassMTL() override;

  id<MTLRenderCommandEncoder> GetNativeEncoder() { return mEncoder; }

 private:
  id<MTLRenderCommandEncoder> mEncoder;
};

}  // namespace cubic
