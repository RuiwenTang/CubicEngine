#include <cubic/render/render_pipeline.h>

#include <vector>

#include "render/vk/vulkan_device.h"

namespace cubic {

class RenderPipelineVK : public RenderPipeline {
 public:
  RenderPipelineVK(VulkanDevice* device, VkPipeline pipeline, VkPipelineLayout layout,
                   std::vector<VkDescriptorSetLayout> slotLayouts);

  ~RenderPipelineVK() override;

  static std::shared_ptr<RenderPipelineVK> Create(VulkanDevice* device, RenderPipelineDescriptor* desc);

 private:
  VulkanDevice* mDevice;
  VkPipeline mPipeline;
  VkPipelineLayout mLayout;
  std::vector<VkDescriptorSetLayout> mSlotLayouts;
};

}  // namespace cubic
