#include "render/vk/command_buffer_vk.h"

#include <cubic/core/log.h>

#include <optional>
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
  uint32_t width = 0;
  uint32_t height = 0;

  std::vector<VkRenderingAttachmentInfo> attachment_infos{};
  std::optional<VkRenderingAttachmentInfo> depth_attachment_info{};
  std::optional<VkRenderingAttachmentInfo> stencil_attachment_info{};

  std::vector<VkViewport> viewports{};
  std::vector<VkRect2D> scissors{};

  for (size_t i = 0; i < desc.colorAttachmentCount; i++) {
    auto attachment = desc.pColorAttachments + i;

    width = attachment->target->GetDescriptor().width;
    height = attachment->target->GetDescriptor().height;

    viewports.emplace_back(VkViewport{0, 0, width * 1.f, height * 1.f, 0.f, 1.f});

    scissors.emplace_back(VkRect2D{{0, 0}, {width, height}});

    vk::AttachmentBuilder builder(dynamic_cast<TextureVK*>(attachment->target.get()),
                                  dynamic_cast<TextureVK*>(attachment->resolveTarget.get()));

    builder.SetLoadOp(attachment->loadOp).SetStoreOp(attachment->storeOp);

    attachment_infos.emplace_back(builder.Build());

    attachment_infos.back().clearValue = vk::TypeConvert(attachment->clearValue);
  }

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

}  // namespace cubic
