#pragma once

#include <cubic/render/render_pass.h>
#include <volk.h>

namespace cubic {

class VulkanDevice;

class RenderPassVK : public RenderPass {
 public:
  RenderPassVK(VulkanDevice* device, VkFramebuffer framebuffer, VkRenderPass renderPass, VkCommandBuffer cmd);

  ~RenderPassVK() override;

  void BindPipeline(const std::shared_ptr<RenderPipeline>& pipeline) override;

  void Draw(uint32_t numVertex, uint32_t firstVertex) override;

  VkCommandBuffer GetNativeCMD() const { return mCMD; }

 private:
  VulkanDevice* mDevice;
  VkFramebuffer mFramebuffer;
  VkRenderPass mRenderPass;
  VkCommandBuffer mCMD;
};

}  // namespace cubic
