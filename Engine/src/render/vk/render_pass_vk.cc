#include "render/vk/render_pass_vk.h"

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

void RenderPassVK::Draw(uint32_t numVertex, uint32_t firstVertex) { vkCmdDraw(mCMD, numVertex, 1, firstVertex, 0); }

}  // namespace cubic
