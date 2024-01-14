#pragma once

#include <cubic/platform.h>
#include <cubic/render/texture.h>
#include <cubic/render/types.h>

#include <memory>

namespace cubic {

struct ColorAttachment {
  std::shared_ptr<Texture> target = {};
  std::shared_ptr<Texture> resolveTarget = {};

  LoadOp loadOp = LoadOp::kUndefined;
  StoreOp storeOp = StoreOp::kUndefined;

  GPUColor clearValue = {};
};

struct DepthStencilAttachment {
  std::shared_ptr<Texture> target = {};
  LoadOp depthLoadOp = LoadOp::kUndefined;
  StoreOp depthStoreOp = StoreOp::kUndefined;
  float depthClearValue = 0.f;

  LoadOp stencilLoadOp = LoadOp::kUndefined;
  StoreOp stencilStoreOp = StoreOp::kUndefined;
  uint32_t stencilClearValue = 0;
};

struct RenderPassDescriptor {
  size_t colorAttachmentCount = 0;
  const ColorAttachment* pColorAttachments = nullptr;
  const DepthStencilAttachment* pDepthStencilAttachment = nullptr;
};

class CUBIC_API RenderPass {
 public:
  virtual ~RenderPass() = default;
};

}  // namespace cubic
