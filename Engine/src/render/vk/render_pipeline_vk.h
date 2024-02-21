#include <cubic/render/render_pipeline.h>

#include <vector>

#include "render/vk/vulkan_device.h"

namespace cubic {

class RenderPipelineVK : public RenderPipeline {
 public:
  RenderPipelineVK(std::shared_ptr<PipelineLayout> layout, VulkanDevice* device, VkPipeline pipeline);

  ~RenderPipelineVK() override;

  static std::shared_ptr<RenderPipelineVK> Create(VulkanDevice* device, RenderPipelineDescriptor* desc);

  void Bind(VkCommandBuffer cmd);

 private:
  VulkanDevice* mDevice;
  VkPipeline mPipeline;
};

}  // namespace cubic
