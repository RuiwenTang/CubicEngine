#include "render/vk/render_pass_vk.h"

#include "render/vk/bind_group_vk.h"
#include "render/vk/buffer_vk.h"
#include "render/vk/render_pipeline_vk.h"
#include "render/vk/vulkan_device.h"

namespace cubic {

RenderPassVK::RenderPassVK(VulkanDevice* device, VkCommandBuffer cmd) : mDevice(device), mCMD(cmd) {}

void RenderPassVK::BindPipeline(const std::shared_ptr<RenderPipeline>& pipeline) {
  auto vk_pipeline = dynamic_cast<RenderPipelineVK*>(pipeline.get());

  vk_pipeline->Bind(mCMD);
}

void RenderPassVK::SetVertexBuffer(const std::shared_ptr<Buffer>& buffer, uint32_t slot, uint64_t offset) {
  auto buffer_vk = dynamic_cast<BufferVK*>(buffer.get());
  auto native_buffer = buffer_vk->GetNativeBuffer();

  vkCmdBindVertexBuffers(mCMD, slot, 1, &native_buffer, &offset);
}

void RenderPassVK::SetIndexBuffer(const std::shared_ptr<Buffer>& buffer, uint64_t offset) {
  auto buffer_vk = dynamic_cast<BufferVK*>(buffer.get());
  auto native_buffer = buffer_vk->GetNativeBuffer();

  vkCmdBindIndexBuffer(mCMD, native_buffer, offset, VK_INDEX_TYPE_UINT32);
}

void RenderPassVK::SetBindGroup(uint32_t slot, const std::shared_ptr<BindGroup>& group) {
  // TODO: implement this function
  auto group_vk = dynamic_cast<BindGroupVK*>(group.get());

  if (group_vk == nullptr) {
    return;
  }

  auto descriptorSet = group_vk->GetNativeSet();

  vkCmdBindDescriptorSets(mCMD, VK_PIPELINE_BIND_POINT_GRAPHICS, group_vk->GetPipelineLayout(), slot, 1, &descriptorSet,
                          0, nullptr);
}

void RenderPassVK::Draw(uint32_t numVertex, uint32_t firstVertex) { vkCmdDraw(mCMD, numVertex, 1, firstVertex, 0); }

void RenderPassVK::DrawElements(uint32_t numIndices, uint32_t firstIndex) {
  vkCmdDrawIndexed(mCMD, numIndices, 1, firstIndex, 0, 0);
}

}  // namespace cubic
