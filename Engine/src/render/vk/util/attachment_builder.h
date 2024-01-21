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
  AttachmentBuilder& SetStencilLoadOp(LoadOp op);
  AttachmentBuilder& SetStencilStoreOp(StoreOp op);

  void Build(std::vector<VkAttachmentDescription>& attachments, std::vector<VkAttachmentReference>& attachmentRefs,
             std::vector<VkAttachmentReference>& resolveRefs);

 private:
  TextureVK* mTarget;
  TextureVK* mResolveTarget;
  LoadOp mLoadOp = LoadOp::kClear;
  StoreOp mStoreOp = StoreOp::kDiscard;
  LoadOp mStencilLoadOp = LoadOp::kUndefined;
  StoreOp mStencilStoreOp = StoreOp::kUndefined;
};

}  // namespace vk
}  // namespace cubic
