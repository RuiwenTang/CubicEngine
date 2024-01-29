#include "render/vk/util/attachment_builder.h"

#include "render/vk/vulkan_types.h"

namespace cubic {
namespace vk {

AttachmentBuilder::AttachmentBuilder(TextureVK* target, TextureVK* resolveTarget)
    : mTarget(target), mResolveTarget(resolveTarget) {}

AttachmentBuilder& AttachmentBuilder::SetLoadOp(LoadOp op) {
  mLoadOp = op;

  return *this;
}

AttachmentBuilder& AttachmentBuilder::SetStoreOp(StoreOp op) {
  mStoreOp = op;

  return *this;
}

VkRenderingAttachmentInfo AttachmentBuilder::Build() {
  VkRenderingAttachmentInfo info{};
  info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;

  bool has_resolve = false;
  if (mResolveTarget) {
    has_resolve = true;

    info.resolveImageView = mResolveTarget->GetImageView();
    info.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;

    info.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }

  info.imageView = mTarget->GetImageView();
  info.loadOp = vk::TypeConvert(mLoadOp);
  info.storeOp = vk::TypeConvert(mStoreOp);

  if (!has_resolve) {
    info.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  } else {
    info.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  }

  return info;
}

}  // namespace vk
}  // namespace cubic
