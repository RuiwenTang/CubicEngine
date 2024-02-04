#include "render/mtl/command_buffer_mtl.h"
#include "render/mtl/buffer_mtl.h"
#include "render/mtl/mtl_types.h"
#include "render/mtl/render_pass_mtl.h"
#include "render/mtl/texture_mtl.h"

namespace cubic {

CommandBufferMTL::CommandBufferMTL(id<MTLCommandBuffer> cmd) : mCMD(cmd) {}

CommandBufferMTL::~CommandBufferMTL() { [mCMD release]; }

std::unique_ptr<RenderPass> CommandBufferMTL::BeginRenderPass(const RenderPassDescriptor& desc) {
  MTLRenderPassDescriptor* mtl_desc = [MTLRenderPassDescriptor new];

  // color attachments
  for (size_t i = 0; i < desc.colorAttachmentCount; i++) {
    auto attachment = desc.pColorAttachments + i;

    mtl_desc.colorAttachments[i].texture = dynamic_cast<TextureMTL*>(attachment->target.get())->GetNativeTexture();
    if (attachment->resolveTarget) {
      mtl_desc.colorAttachments[i].resolveTexture =
          dynamic_cast<TextureMTL*>(attachment->resolveTarget.get())->GetNativeTexture();
    }

    mtl_desc.colorAttachments[i].clearColor = TypeConvert(attachment->clearValue);
    mtl_desc.colorAttachments[i].loadAction = TypeConvert(attachment->loadOp);
    mtl_desc.colorAttachments[i].storeAction = TypeConvert(attachment->storeOp);
  }

  if (desc.pDepthStencilAttachment) {
    auto format = desc.pDepthStencilAttachment->target->GetDescriptor().format;
    if (format == TextureFormat::kDepth24Stencil8 || format == TextureFormat::kDepth24) {
      mtl_desc.depthAttachment.texture =
          dynamic_cast<TextureMTL*>(desc.pDepthStencilAttachment->target.get())->GetNativeTexture();

      if (desc.pDepthStencilAttachment->resolveTarget) {
        mtl_desc.depthAttachment.resolveTexture =
            dynamic_cast<TextureMTL*>(desc.pDepthStencilAttachment->resolveTarget.get())->GetNativeTexture();
      }

      mtl_desc.depthAttachment.clearDepth = desc.pDepthStencilAttachment->depthClearValue;
      mtl_desc.depthAttachment.loadAction = TypeConvert(desc.pDepthStencilAttachment->depthLoadOp);
      mtl_desc.depthAttachment.storeAction = TypeConvert(desc.pDepthStencilAttachment->depthStoreOp);
    }

    if (format == TextureFormat::kDepth24Stencil8 || format == TextureFormat::kStencil8) {
      mtl_desc.stencilAttachment.texture =
          dynamic_cast<TextureMTL*>(desc.pDepthStencilAttachment->target.get())->GetNativeTexture();

      if (desc.pDepthStencilAttachment->resolveTarget) {
        mtl_desc.stencilAttachment.resolveTexture =
            dynamic_cast<TextureMTL*>(desc.pDepthStencilAttachment->resolveTarget.get())->GetNativeTexture();
      }

      mtl_desc.stencilAttachment.clearStencil = desc.pDepthStencilAttachment->stencilClearValue;
      mtl_desc.stencilAttachment.loadAction = TypeConvert(desc.pDepthStencilAttachment->stencilLoadOp);
      mtl_desc.stencilAttachment.storeAction = TypeConvert(desc.pDepthStencilAttachment->stencilStoreOp);
    }
  }

  id<MTLRenderCommandEncoder> encoder = [mCMD renderCommandEncoderWithDescriptor:mtl_desc];

  [mtl_desc release];

  if (encoder == nil) {
    return {};
  }

  return std::make_unique<RenderPassMTL>(encoder);
}

void CommandBufferMTL::EndRenderPass(std::unique_ptr<RenderPass> render_pass) {
  auto native_render_pass = dynamic_cast<RenderPassMTL*>(render_pass.get());

  if (native_render_pass == nullptr) {
    return;
  }

  [native_render_pass->GetNativeEncoder() endEncoding];
}

void CommandBufferMTL::CopyBufferToBuffer(const std::shared_ptr<Buffer>& dst, uint64_t dst_offset,
                                          const std::shared_ptr<Buffer>& src, uint64_t src_offset, uint64_t length) {
  auto mtl_dst = dynamic_cast<BufferMTL*>(dst.get());
  auto mtl_src = dynamic_cast<BufferMTL*>(src.get());

  if (mtl_dst == nullptr || mtl_src == nullptr) {
    return;
  }

  id<MTLBlitCommandEncoder> encoder = [mCMD blitCommandEncoder];

  [encoder copyFromBuffer:mtl_src->GetNativeBuffer()
             sourceOffset:src_offset
                 toBuffer:mtl_dst->GetNativeBuffer()
        destinationOffset:dst_offset
                     size:length];

  [encoder endEncoding];

  [encoder release];
}

}
