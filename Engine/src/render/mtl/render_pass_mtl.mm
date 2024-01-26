#include "render/mtl/render_pass_mtl.h"
#include "render/mtl/render_pipeline_mtl.h"

namespace cubic {

RenderPassMTL::RenderPassMTL(id<MTLRenderCommandEncoder> encoder) : mEncoder(encoder) {}

RenderPassMTL::~RenderPassMTL() { [mEncoder release]; }

void RenderPassMTL::BindPipeline(const std::shared_ptr<RenderPipeline> &pipeline) {
  auto mtl_pipeline = dynamic_cast<RenderPipelineMTL *>(pipeline.get());

  mtl_pipeline->BindToEncoder(mEncoder);
}

void RenderPassMTL::Draw(uint32_t numVertex, uint32_t firstVertex) {
  [mEncoder drawPrimitives:MTLPrimitiveTypeTriangle
               vertexStart:firstVertex
               vertexCount:numVertex
             instanceCount:1
              baseInstance:0];
}

}
