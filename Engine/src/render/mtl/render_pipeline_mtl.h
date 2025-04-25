#pragma once

#import <Metal/Metal.h>
#include <cubic/render/render_pipeline.h>

#include "render/mtl/pipeline_layout_mtl.h"

namespace cubic {

class RenderPipelineMTL : public RenderPipeline {
 public:
  RenderPipelineMTL(id<MTLRenderPipelineState> pipeline, id<MTLDepthStencilState> depthStencilState, PipelineLayoutMTL layout);

  ~RenderPipelineMTL() override;

  const PipelineLayout* GetLayout() const override;

  void BindToEncoder(id<MTLRenderCommandEncoder> encoder);

  static std::shared_ptr<RenderPipelineMTL> Create(RenderPipelineDescriptor* desc, id<MTLDevice> device);

 private:
  id<MTLRenderPipelineState> mNativePipeline;
  id<MTLDepthStencilState> mDepthStencilState;
  PipelineLayoutMTL mLayout;
};

}  // namespace cubic
