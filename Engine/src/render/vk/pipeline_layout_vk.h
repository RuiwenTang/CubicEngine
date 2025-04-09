#pragma once

#include <cubic/render/pipeline_layout.h>
#include <volk.h>

namespace cubic {

class VulkanDevice;

class PipelineLayoutVK : public PipelineLayout {
 public:
  PipelineLayoutVK(VulkanDevice* device, VkPipelineLayout layout, std::vector<VkDescriptorSetLayout> setLayouts);

  ~PipelineLayoutVK() override;

  VkPipelineLayout GetNativeLayout() const { return mLayout; }

  static std::unique_ptr<PipelineLayout> Create(const std::vector<BindGroupLayout>& groups, VulkanDevice* device);

 private:
  VulkanDevice* mDevice;
  VkPipelineLayout mLayout;
  std::vector<VkDescriptorSetLayout> mSetLayouts;
};

}  // namespace cubic
