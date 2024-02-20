#include "render/mtl/render_pipeline_mtl.h"

#include <cubic/core/log.h>

#include "render/mtl/mtl_types.h"
#include "render/mtl/shader_module_mtl.h"

namespace cubic {

RenderPipelineMTL::RenderPipelineMTL(std::shared_ptr<PipelineLayout> layout, id<MTLRenderPipelineState> pipeline,
                                     id<MTLDepthStencilState> depthStencilState)
    : RenderPipeline(std::move(layout)), mNativePipeline(pipeline), mDepthStencilState(depthStencilState) {}

RenderPipelineMTL::~RenderPipelineMTL() {
  [mDepthStencilState release];

  [mNativePipeline release];
}

void RenderPipelineMTL::BindToEncoder(id<MTLRenderCommandEncoder> encoder) {
  [encoder setRenderPipelineState:mNativePipeline];
  if (mDepthStencilState) {
    [encoder setDepthStencilState:mDepthStencilState];
  }
}

std::shared_ptr<RenderPipelineMTL> RenderPipelineMTL::Create(RenderPipelineDescriptor* desc, id<MTLDevice> device) {
  MTLRenderPipelineDescriptor* mtl_desc = [MTLRenderPipelineDescriptor new];

  mtl_desc.vertexFunction = dynamic_cast<ShaderModuleMTL*>(desc->vertexShader.get())->GetEntryPoint();
  mtl_desc.fragmentFunction = dynamic_cast<ShaderModuleMTL*>(desc->fragmentShader.get())->GetEntryPoint();
  mtl_desc.rasterSampleCount = desc->sampleCount;

  if (!desc->vertexBuffer.empty()) {
    mtl_desc.vertexDescriptor = [MTLVertexDescriptor new];

    for (size_t i = 0; i < desc->vertexBuffer.size(); i++) {
      mtl_desc.vertexDescriptor.layouts[i + kVertexIndexOffset].stride = desc->vertexBuffer[i].stride;
      mtl_desc.vertexDescriptor.layouts[i + kVertexIndexOffset].stepFunction =
          TypeConvert(desc->vertexBuffer[i].stepMode);
      mtl_desc.vertexDescriptor.layouts[i + kVertexIndexOffset].stepRate = 1;

      for (size_t j = 0; j < desc->vertexBuffer[i].attribute.size(); j++) {
        const auto& attr = desc->vertexBuffer[i].attribute[j];
        mtl_desc.vertexDescriptor.attributes[attr.location].format = TypeConvert(attr.format);
        mtl_desc.vertexDescriptor.attributes[attr.location].offset = attr.offset;
        mtl_desc.vertexDescriptor.attributes[attr.location].bufferIndex = i + kVertexIndexOffset;
      }
    }
  }

  for (uint32_t i = 0; i < desc->colorCount; i++) {
    mtl_desc.colorAttachments[i].pixelFormat = TypeConvert(desc->pColorTargets[i].format);
    auto blending = desc->pColorTargets[i].blend;
    if (blending) {
      mtl_desc.colorAttachments[i].blendingEnabled = YES;
      // color
      mtl_desc.colorAttachments[i].rgbBlendOperation = TypeConvert(blending->color.operation);
      mtl_desc.colorAttachments[i].sourceRGBBlendFactor = TypeConvert(blending->color.srcFactor);
      mtl_desc.colorAttachments[i].destinationRGBBlendFactor = TypeConvert(blending->color.dstFactor);
      // alpha
      mtl_desc.colorAttachments[i].alphaBlendOperation = TypeConvert(blending->alpha.operation);
      mtl_desc.colorAttachments[i].sourceAlphaBlendFactor = TypeConvert(blending->alpha.srcFactor);
      mtl_desc.colorAttachments[i].destinationAlphaBlendFactor = TypeConvert(blending->alpha.dstFactor);
    } else {
      mtl_desc.colorAttachments[i].blendingEnabled = NO;
    }
  }

  id<MTLDepthStencilState> depthStencilState = nil;
  if (desc->depthStencil) {
    // TODO handle depth stencil state
  } else {
    mtl_desc.depthAttachmentPixelFormat = MTLPixelFormatInvalid;
  }

  NSError* error = nil;

  id<MTLRenderPipelineState> pipeline = [device newRenderPipelineStateWithDescriptor:mtl_desc error:&error];

  [mtl_desc release];

  if (error) {
    CUB_ERROR("[Metal backend] pipeline create failed: {} ", [[error localizedDescription] UTF8String]);

    [error release];

    return {};
  }

  return std::make_shared<RenderPipelineMTL>(desc->layout, pipeline, depthStencilState);
}

}  // namespace cubic
