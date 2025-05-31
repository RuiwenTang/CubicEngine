#pragma once

#include <cubic/platform.h>
#include <cubic/render/buffer.h>
#include <cubic/render/render_pass.h>

namespace cubic {

class CUBIC_API CommandBuffer {
 public:
  virtual ~CommandBuffer() = default;

  virtual std::unique_ptr<RenderPass> BeginRenderPass(const RenderPassDescriptor& desc) = 0;

  virtual void EndRenderPass(std::unique_ptr<RenderPass> render_pass) = 0;

  virtual void CopyBufferToBuffer(const std::shared_ptr<Buffer>& dst, uint64_t dst_offset,
                                  const std::shared_ptr<Buffer>& src, uint64_t src_offset, uint64_t length) = 0;

  virtual void CopyBufferToTexture(const std::shared_ptr<Buffer>& src, uint64_t src_offset,
                                   const std::shared_ptr<Texture>& texture, const Region& dst_region) = 0;
};

}  // namespace cubic
