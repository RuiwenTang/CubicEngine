#include "render/vk/command_buffer_vk.h"

#include <cubic/core/log.h>

#include <optional>
#include <vector>

#include "render/vk/buffer_vk.h"
#include "render/vk/render_pass_vk.h"
#include "render/vk/texture_vk.h"
#include "render/vk/util/attachment_builder.h"
#include "render/vk/vulkan_device.h"
#include "render/vk/vulkan_types.h"

namespace cubic {

CommandBufferVK::CommandBufferVK(VulkanDevice* device, VkCommandBuffer cmd, uint64_t value)
    : mDevice(device), mCmd(cmd), mSignalValue(value) {}

CommandBufferVK::~CommandBufferVK() { mPendingResources.clear(); }

std::unique_ptr<RenderPass> CommandBufferVK::BeginRenderPass(const RenderPassDescriptor& desc) {
  uint32_t width = 0;
  uint32_t height = 0;

  std::vector<VkRenderingAttachmentInfo> attachment_infos{};
  std::optional<VkRenderingAttachmentInfo> depth_attachment_info{};
  std::optional<VkRenderingAttachmentInfo> stencil_attachment_info{};

  std::vector<VkViewport> viewports{};
  std::vector<VkRect2D> scissors{};

  std::vector<VkImageMemoryBarrier> attachment_barraiers{};

  for (size_t i = 0; i < desc.colorAttachmentCount; i++) {
    auto attachment = desc.pColorAttachments + i;

    width = attachment->target->GetDescriptor().width;
    height = attachment->target->GetDescriptor().height;

    viewports.emplace_back(VkViewport{0, 0, width * 1.f, height * 1.f, 0.f, 1.f});

    scissors.emplace_back(VkRect2D{{0, 0}, {width, height}});

    auto target = dynamic_cast<TextureVK*>(attachment->target.get());
    auto resolve_target = dynamic_cast<TextureVK*>(attachment->resolveTarget.get());

    vk::AttachmentBuilder builder(target, resolve_target);

    builder.SetLoadOp(attachment->loadOp).SetStoreOp(attachment->storeOp);

    attachment_infos.emplace_back(builder.Build());

    attachment_infos.back().clearValue = vk::TypeConvert(attachment->clearValue);

    if (resolve_target && resolve_target->GetImageLayout() != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
      VkImageMemoryBarrier barrier{};
      barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;

      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = 0;
      barrier.oldLayout = resolve_target->GetImageLayout();
      barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      barrier.image = resolve_target->GetImage();

      resolve_target->SetImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

      attachment_barraiers.emplace_back(barrier);
    }

    if (target->GetImageLayout() != VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
      VkImageMemoryBarrier barrier{};
      barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
      barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
      barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

      barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      barrier.subresourceRange.baseMipLevel = 0;
      barrier.subresourceRange.levelCount = 1;
      barrier.subresourceRange.baseArrayLayer = 0;
      barrier.subresourceRange.layerCount = 1;

      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = 0;
      barrier.oldLayout = target->GetImageLayout();
      barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
      barrier.image = target->GetImage();

      target->SetImageLayout(VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

      attachment_barraiers.emplace_back(barrier);
    }
  }

  // change layouts
  vkCmdPipelineBarrier(mCmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0,
                       nullptr, static_cast<uint32_t>(attachment_barraiers.size()), attachment_barraiers.data());

  VkRenderingInfo rendering_info{};
  rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
  rendering_info.renderArea.extent.width = width;
  rendering_info.renderArea.extent.height = height;
  rendering_info.layerCount = 1;
  rendering_info.colorAttachmentCount = static_cast<uint32_t>(attachment_infos.size());
  rendering_info.pColorAttachments = attachment_infos.data();

  vkCmdBeginRendering(mCmd, &rendering_info);

  vkCmdSetViewport(mCmd, 0, static_cast<uint32_t>(viewports.size()), viewports.data());
  vkCmdSetScissor(mCmd, 0, static_cast<uint32_t>(scissors.size()), scissors.data());

  return std::make_unique<RenderPassVK>(mDevice, mCmd);
}

void CommandBufferVK::EndRenderPass(std::unique_ptr<RenderPass> render_pass) {
  auto render_pass_vk = dynamic_cast<RenderPassVK*>(render_pass.get());

  if (render_pass_vk == nullptr) {
    return;
  }

  if (render_pass_vk->GetNativeCMD() != mCmd) {
    return;
  }

  vkCmdEndRendering(mCmd);
}

void CommandBufferVK::CopyBufferToBuffer(const std::shared_ptr<Buffer>& dst, uint64_t dst_offset,
                                         const std::shared_ptr<Buffer>& src, uint64_t src_offset, uint64_t length) {
  auto buffer_dst = dynamic_cast<BufferVK*>(dst.get());
  auto buffer_src = dynamic_cast<BufferVK*>(src.get());

  VkBufferCopy copy_region{src_offset, dst_offset, length};

  vkCmdCopyBuffer(mCmd, buffer_src->GetNativeBuffer(), buffer_dst->GetNativeBuffer(), 1, &copy_region);

  RecordResource(src);

}  // namespace cubic

void CommandBufferVK::RecordResource(const std::shared_ptr<Buffer>& buffer) {
  for (const auto& res : mPendingResources) {
    if (res == buffer) {
      return;
    }
  }

  mPendingResources.emplace_back(buffer);
}

}  // namespace cubic
