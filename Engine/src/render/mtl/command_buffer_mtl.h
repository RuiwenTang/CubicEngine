#pragma once

#import <Metal/Metal.h>
#include <cubic/render/command_buffer.h>

namespace cubic {

class CommandBufferMTL : public CommandBuffer {
 public:
  CommandBufferMTL(id<MTLCommandBuffer> cmd);

  ~CommandBufferMTL() override;

  std::unique_ptr<RenderPass> BeginRenderPass(const RenderPassDescriptor &desc) override;

  void EndRenderPass(std::unique_ptr<RenderPass> render_pass) override;

  void CopyBufferToBuffer(const std::shared_ptr<Buffer> &dst, uint64_t dst_offset, const std::shared_ptr<Buffer> &src,
                          uint64_t src_offset, uint64_t length) override;

  id<MTLCommandBuffer> GetNativeCMD() { return mCMD; }

 private:
  id<MTLCommandBuffer> mCMD;
};

}  // namespace cubic
