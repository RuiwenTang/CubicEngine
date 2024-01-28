#pragma once

#include <cubic/render/render_pass.h>
#include <volk.h>

#include <vector>

#include "render/vk/texture_vk.h"

namespace cubic {
namespace vk {

class AttachmentBuilder {
 public:
  AttachmentBuilder(TextureVK* target, TextureVK* resolveTarget);

  ~AttachmentBuilder() = default;

  AttachmentBuilder& SetLoadOp(LoadOp op);
  AttachmentBuilder& SetStoreOp(StoreOp op);

  VkRenderingAttachmentInfo Build();

 private:
  TextureVK* mTarget;
  TextureVK* mResolveTarget;
  LoadOp mLoadOp = LoadOp::kClear;
  StoreOp mStoreOp = StoreOp::kDiscard;
};

}  // namespace vk
}  // namespace cubic
