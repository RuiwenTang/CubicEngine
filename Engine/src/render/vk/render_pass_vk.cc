#include "render/vk/render_pass_vk.h"

#include "render/vk/vulkan_device.h"

namespace cubic {

RenderPassVK::RenderPassVK(VulkanDevice* device, VkFramebuffer framebuffer, VkRenderPass renderPass,
                           VkCommandBuffer cmd)
    : mDevice(device), mFramebuffer(framebuffer), mRenderPass(renderPass), mCMD(cmd) {}

RenderPassVK::~RenderPassVK() {
  if (mRenderPass) {
    vkDestroyRenderPass(mDevice->GetLogicalDevice(), mRenderPass, nullptr);
  }

  if (mFramebuffer) {
    vkDestroyFramebuffer(mDevice->GetLogicalDevice(), mFramebuffer, nullptr);
  }
}

void RenderPassVK::BindPipeline(const std::shared_ptr<RenderPipeline>& pipeline) {}

void RenderPassVK::Draw(uint32_t numVertex, uint32_t firstVertex) {}

}  // namespace cubic
