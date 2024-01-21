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

AttachmentBuilder& AttachmentBuilder::SetStencilLoadOp(LoadOp op) {
  mStencilLoadOp = op;

  return *this;
}

AttachmentBuilder& AttachmentBuilder::SetStencilStoreOp(StoreOp op) {
  mStencilStoreOp = op;

  return *this;
}

void AttachmentBuilder::Build(std::vector<VkAttachmentDescription>& attachments,
                              std::vector<VkAttachmentReference>& attachmentRefs,
                              std::vector<VkAttachmentReference>& resolveRefs) {
  bool has_resolve = false;
  if (mResolveTarget) {
    has_resolve = true;

    VkAttachmentDescription desc{};
    desc.format = vk::TypeConvert(mResolveTarget->GetDescriptor().format);
    desc.samples = VK_SAMPLE_COUNT_1_BIT;
    desc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    desc.storeOp = vk::TypeConvert(mStoreOp);
    desc.initialLayout = mResolveTarget->GetImageLayout();
    desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    desc.stencilStoreOp = vk::TypeConvert(mStencilStoreOp);

    if (mResolveTarget->GetTextureSource() == TextureVK::Source::kSwapchain) {
      desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    } else if (mResolveTarget->GetDescriptor().usage & TextureUsage::kShaderRead) {
      desc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    } else {
      desc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    mResolveTarget->SetImageLayout(desc.finalLayout);

    attachments.emplace_back(desc);

    VkAttachmentReference ref{};
    ref.attachment = attachments.size() - 1;
    ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    resolveRefs.emplace_back(ref);
  }

  VkAttachmentDescription desc{};

  desc.format = vk::TypeConvert(mTarget->GetDescriptor().format);
  desc.samples = vk::TypeConvert(mTarget->GetDescriptor().sample_count);
  desc.loadOp = vk::TypeConvert(mLoadOp);
  desc.initialLayout = mTarget->GetImageLayout();
  desc.stencilLoadOp = vk::TypeConvert(mStencilLoadOp);

  if (has_resolve) {
    desc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  } else {
    desc.storeOp = vk::TypeConvert(mStoreOp);
    desc.stencilStoreOp = vk::TypeConvert(mStencilStoreOp);
  }

  if (mTarget->GetTextureSource() == TextureVK::Source::kSwapchain) {
    desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  } else if (mTarget->GetDescriptor().usage & TextureUsage::kShaderRead) {
    desc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  } else {
    desc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }

  mTarget->SetImageLayout(desc.finalLayout);

  attachments.emplace_back(desc);

  VkAttachmentReference ref{};
  ref.attachment = attachments.size() - 1;
  ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  attachmentRefs.emplace_back(ref);

  if (!has_resolve) {
    VkAttachmentReference rf{};
    rf.attachment = VK_ATTACHMENT_UNUSED;
    rf.layout = VK_IMAGE_LAYOUT_UNDEFINED;

    resolveRefs.emplace_back(rf);
  }
}

}  // namespace vk
}  // namespace cubic
