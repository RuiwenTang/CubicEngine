#include "render/vk/render_pass_vk.h"

#include "render/vk/render_pipeline_vk.h"
#include "render/vk/vulkan_device.h"

namespace cubic {

RenderPassVK::RenderPassVK(VulkanDevice* device, VkCommandBuffer cmd) : mDevice(device), mCMD(cmd) {}

void RenderPassVK::BindPipeline(const std::shared_ptr<RenderPipeline>& pipeline) {
  auto vk_pipeline = dynamic_cast<RenderPipelineVK*>(pipeline.get());

  vk_pipeline->Bind(mCMD);
}

void RenderPassVK::Draw(uint32_t numVertex, uint32_t firstVertex) { vkCmdDraw(mCMD, numVertex, 1, firstVertex, 0); }

}  // namespace cubic
