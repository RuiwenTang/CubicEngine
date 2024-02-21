#pragma once

#include <cubic/render/render_pass.h>
#include <volk.h>

namespace cubic {

class VulkanDevice;

class RenderPassVK : public RenderPass {
 public:
  RenderPassVK(VulkanDevice* device, VkCommandBuffer cmd);

  ~RenderPassVK() override = default;

  void BindPipeline(const std::shared_ptr<RenderPipeline>& pipeline) override;

  void SetVertexBuffer(const std::shared_ptr<Buffer>& buffer, uint32_t slot, uint64_t offset) override;

  void SetIndexBuffer(const std::shared_ptr<Buffer>& buffer, uint64_t offset) override;

  void SetBindGroup(uint32_t slot, const std::shared_ptr<BindGroup>& group) override;

  void Draw(uint32_t numVertex, uint32_t firstVertex) override;

  void DrawElements(uint32_t numIndices, uint32_t firstIndex) override;

  VkCommandBuffer GetNativeCMD() const { return mCMD; }

 private:
  VulkanDevice* mDevice;
  VkCommandBuffer mCMD;
};

}  // namespace cubic
