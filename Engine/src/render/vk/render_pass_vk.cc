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

}  // namespace cubic
