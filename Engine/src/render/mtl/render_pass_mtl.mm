#include "render/mtl/render_pass_mtl.h"
#include "render/mtl/buffer_mtl.h"
#include "render/mtl/mtl_types.h"
#include "render/mtl/render_pipeline_mtl.h"

namespace cubic {

RenderPassMTL::RenderPassMTL(id<MTLRenderCommandEncoder> encoder) : mEncoder(encoder) {}

RenderPassMTL::~RenderPassMTL() { [mEncoder release]; }

void RenderPassMTL::BindPipeline(const std::shared_ptr<RenderPipeline> &pipeline) {
  auto mtl_pipeline = dynamic_cast<RenderPipelineMTL *>(pipeline.get());

  mtl_pipeline->BindToEncoder(mEncoder);
}

void RenderPassMTL::SetVertexBuffer(const std::shared_ptr<Buffer> &buffer, uint32_t slot, uint64_t offset) {
  auto mtl_buffer = dynamic_cast<BufferMTL *>(buffer.get());

  if (mtl_buffer == nullptr) {
    return;
  }

  [mEncoder setVertexBuffer:mtl_buffer->GetNativeBuffer() offset:offset atIndex:slot + kVertexIndexOffset];
}

void RenderPassMTL::SetIndexBuffer(const std::shared_ptr<Buffer> &buffer, uint64_t offset) {
  auto mtl_buffer = dynamic_cast<BufferMTL *>(buffer.get());

  if (mtl_buffer == nullptr) {
    return;
  }

  mCurrIndex = buffer;
  mCurrIndexOffset = offset;
}

void RenderPassMTL::SetBindGroup(const PipelineLayout *layout, uint32_t slot, const BindGroup &group) {
  auto mtl_layout = dynamic_cast<const PipelineLayoutMTL *>(layout);
  if (mtl_layout == nullptr) {
    return;
  }


  mtl_layout->SetBindGroup(mEncoder, slot, group);
}

void RenderPassMTL::DrawElements(uint32_t numIndices, uint32_t firstIndex) {
  if (mCurrIndex == nullptr) {
    return;
  }

  id<MTLBuffer> buffer = dynamic_cast<BufferMTL *>(mCurrIndex.get())->GetNativeBuffer();

  [mEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                       indexCount:numIndices
                        indexType:MTLIndexTypeUInt32
                      indexBuffer:buffer
                indexBufferOffset:mCurrIndexOffset + sizeof(uint32_t) * firstIndex];
}

void RenderPassMTL::Draw(uint32_t numVertex, uint32_t firstVertex) {
  [mEncoder drawPrimitives:MTLPrimitiveTypeTriangle
               vertexStart:firstVertex
               vertexCount:numVertex
             instanceCount:1
              baseInstance:0];
}

}  // namespace cubic
