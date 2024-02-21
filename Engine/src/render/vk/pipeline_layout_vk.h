#pragma once

#include <cubic/render/pipeline_layout.h>
#include <volk.h>

namespace cubic {

class VulkanDevice;

class PipelineLayoutVK : public PipelineLayout {
 public:
  PipelineLayoutVK(std::vector<std::shared_ptr<BindGroupLayout>> groups, VulkanDevice* device, VkPipelineLayout layout);

  ~PipelineLayoutVK() override;

  VkPipelineLayout GetNativeLayout() const { return mLayout; }

  static std::shared_ptr<PipelineLayout> Create(std::vector<std::shared_ptr<BindGroupLayout>> groups,
                                                VulkanDevice* device);

 private:
  VulkanDevice* mDevice;
  VkPipelineLayout mLayout;
};

}  // namespace cubic
