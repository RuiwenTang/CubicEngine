#pragma once

#import <Metal/Metal.h>
#include <cubic/render/render_pipeline.h>

namespace cubic {

class RenderPipelineMTL : public RenderPipeline {
 public:
  RenderPipelineMTL(id<MTLRenderPipelineState> pipeline, id<MTLDepthStencilState> depthStencilState);

  ~RenderPipelineMTL() override;

  void BindToEncoder(id<MTLRenderCommandEncoder> encoder);

  static std::shared_ptr<RenderPipelineMTL> Create(RenderPipelineDescriptor* desc, id<MTLDevice> device);

 private:
  id<MTLRenderPipelineState> mNativePipeline;
  id<MTLDepthStencilState> mDepthStencilState;
};

}  // namespace cubic
