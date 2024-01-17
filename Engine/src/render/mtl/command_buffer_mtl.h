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

  id<MTLCommandBuffer> GetNativeCMD() { return mCMD; }

 private:
  id<MTLCommandBuffer> mCMD;
};

}  // namespace cubic
