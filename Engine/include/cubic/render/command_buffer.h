#pragma once

#include <cubic/platform.h>
#include <cubic/render/render_pass.h>

namespace cubic {

class CUBIC_API CommandBuffer {
 public:
  virtual ~CommandBuffer() = default;

  virtual std::unique_ptr<RenderPass> BeginRenderPass(const RenderPassDescriptor& desc) = 0;

  virtual void EndRenderPass(std::unique_ptr<RenderPass> render_pass) = 0;
};

}  // namespace cubic
