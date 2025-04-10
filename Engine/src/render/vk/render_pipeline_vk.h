#include <cubic/render/render_pipeline.h>

#include <vector>

#include "render/vk/pipeline_layout_vk.h"
#include "render/vk/vulkan_device.h"

namespace cubic {

class RenderPipelineVK : public RenderPipeline {
 public:
  RenderPipelineVK(VulkanDevice* device, VkPipeline pipeline, std::unique_ptr<PipelineLayoutVK> layout);

  ~RenderPipelineVK() override;

  const PipelineLayout* GetLayout() const override { return nullptr; }

  static std::shared_ptr<RenderPipelineVK> Create(VulkanDevice* device, RenderPipelineDescriptor* desc);

  void Bind(VkCommandBuffer cmd);

 private:
  VulkanDevice* mDevice;
  VkPipeline mPipeline;
  std::unique_ptr<PipelineLayoutVK> mLayout;
};

}  // namespace cubic
