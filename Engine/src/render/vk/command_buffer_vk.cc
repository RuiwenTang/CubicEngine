#include "render/vk/command_buffer_vk.h"

#include <cubic/core/log.h>

#include <vector>

#include "render/vk/render_pass_vk.h"
#include "render/vk/texture_vk.h"
#include "render/vk/util/attachment_builder.h"
#include "render/vk/vulkan_device.h"
#include "render/vk/vulkan_types.h"

namespace cubic {

CommandBufferVK::CommandBufferVK(VulkanDevice* device, VkCommandBuffer cmd, uint64_t value)
    : mDevice(device), mCmd(cmd), mSignalValue(value) {}

CommandBufferVK::~CommandBufferVK() {}

std::unique_ptr<RenderPass> CommandBufferVK::BeginRenderPass(const RenderPassDescriptor& desc) {
  std::vector<VkAttachmentDescription> attachments{};
  std::vector<VkAttachmentReference> attachment_refs{};
  std::vector<VkAttachmentReference> resolve_refs{};

  uint32_t width = 0;
  uint32_t height = 0;

  for (size_t i = 0; i < desc.colorAttachmentCount; i++) {
    vk::AttachmentBuilder builder(dynamic_cast<TextureVK*>(desc.pColorAttachments[i].target.get()),
                                  dynamic_cast<TextureVK*>(desc.pColorAttachments[i].resolveTarget.get()));

    width = desc.pColorAttachments[0].target->GetDescriptor().width;
    height = desc.pColorAttachments[0].target->GetDescriptor().height;

    builder.SetLoadOp(desc.pColorAttachments[i].loadOp)
        .SetStoreOp(desc.pColorAttachments[i].storeOp)
        .Build(attachments, attachment_refs, resolve_refs);
  }

  if (desc.pDepthStencilAttachment) {
    vk::AttachmentBuilder builder(dynamic_cast<TextureVK*>(desc.pDepthStencilAttachment->target.get()),
                                  dynamic_cast<TextureVK*>(desc.pDepthStencilAttachment->resolveTarget.get()));

    builder.SetLoadOp(desc.pDepthStencilAttachment->depthLoadOp)
        .SetStoreOp(desc.pDepthStencilAttachment->depthStoreOp)
        .SetStencilLoadOp(desc.pDepthStencilAttachment->stencilLoadOp)
        .SetStencilStoreOp(desc.pDepthStencilAttachment->stencilStoreOp)
        .Build(attachments, attachment_refs, resolve_refs);

    if (width == 0) {
      width = desc.pDepthStencilAttachment->target->GetDescriptor().width;
      height = desc.pDepthStencilAttachment->target->GetDescriptor().height;
    }
  }

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

  subpass.colorAttachmentCount = desc.colorAttachmentCount;
  subpass.pColorAttachments = attachment_refs.data();
  subpass.pResolveAttachments = resolve_refs.data();

  if (desc.pDepthStencilAttachment) {
    subpass.pDepthStencilAttachment = attachment_refs.data() + (attachment_refs.size() - 1);
  }

  VkRenderPassCreateInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  render_pass_info.attachmentCount = attachments.size();
  render_pass_info.pAttachments = attachments.data();

  render_pass_info.subpassCount = 1;
  render_pass_info.pSubpasses = &subpass;

  VkRenderPass vk_render_pass = VK_NULL_HANDLE;

  if (vkCreateRenderPass(mDevice->GetLogicalDevice(), &render_pass_info, nullptr, &vk_render_pass) != VK_SUCCESS) {
    CUB_ERROR("Failed create VkRenderPass ... ");
    return {};
  }

  std::vector<VkImageView> framebuffer_views{};

  for (size_t i = 0; i < desc.colorAttachmentCount; i++) {
    auto attachment = desc.pColorAttachments + i;

    if (attachment->resolveTarget) {
      framebuffer_views.emplace_back(dynamic_cast<TextureVK*>(attachment->resolveTarget.get())->GetImageView());
    }

    framebuffer_views.emplace_back(dynamic_cast<TextureVK*>(attachment->target.get())->GetImageView());
  }

  if (desc.pDepthStencilAttachment) {
    if (desc.pDepthStencilAttachment->resolveTarget) {
      framebuffer_views.emplace_back(
          dynamic_cast<TextureVK*>(desc.pDepthStencilAttachment->resolveTarget.get())->GetImageView());
    }

    framebuffer_views.emplace_back(
        dynamic_cast<TextureVK*>(desc.pDepthStencilAttachment->target.get())->GetImageView());
  }

  VkFramebufferCreateInfo fb_info{};
  fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  fb_info.renderPass = vk_render_pass;
  fb_info.attachmentCount = framebuffer_views.size();
  fb_info.pAttachments = framebuffer_views.data();
  fb_info.width = width;
  fb_info.height = height;
  fb_info.layers = 1;

  VkFramebuffer vk_fbo = VK_NULL_HANDLE;

  if (vkCreateFramebuffer(mDevice->GetLogicalDevice(), &fb_info, nullptr, &vk_fbo) != VK_SUCCESS) {
    CUB_ERROR("Failed create VkFramebuffer ... ");
    return {};
  }

  std::vector<VkClearValue> clear_values{};

  for (size_t i = 0; i < desc.colorAttachmentCount; i++) {
    clear_values.emplace_back(vk::TypeConvert(desc.pColorAttachments[i].clearValue));
  }

  if (desc.pDepthStencilAttachment) {
    clear_values.emplace_back(vk::TypeConvert(desc.pDepthStencilAttachment->depthClearValue,
                                              desc.pDepthStencilAttachment->stencilClearValue));
  }

  VkRenderPassBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  begin_info.renderPass = vk_render_pass;
  begin_info.framebuffer = vk_fbo;
  begin_info.renderArea.offset = {0, 0};
  begin_info.renderArea.extent.width = width;
  begin_info.renderArea.extent.height = height;
  begin_info.clearValueCount = clear_values.size();
  begin_info.pClearValues = clear_values.data();

  vkCmdBeginRenderPass(mCmd, &begin_info, VK_SUBPASS_CONTENTS_INLINE);

  return std::make_unique<RenderPassVK>(mDevice, vk_fbo, vk_render_pass, mCmd);
}

void CommandBufferVK::EndRenderPass(std::unique_ptr<RenderPass> render_pass) {
  auto render_pass_vk = dynamic_cast<RenderPassVK*>(render_pass.get());

  if (render_pass_vk == nullptr) {
    return;
  }

  if (render_pass_vk->GetNativeCMD() != mCmd) {
    return;
  }

  vkCmdEndRenderPass(mCmd);

  mPendingRenderPass.emplace_back(std::move(render_pass));
}

}  // namespace cubic
