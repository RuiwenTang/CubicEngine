#pragma once

#include <cubic/render/bind_group.h>
#include <volk.h>

#include <memory>

namespace cubic {

class VulkanDevice;

class BindGroupLayoutVK : public BindGroupLayout {
 public:
  BindGroupLayoutVK(std::vector<GroupEntryInfo> entries, VulkanDevice* device, VkDescriptorSetLayout layout);

  ~BindGroupLayoutVK() override;

  VkDescriptorSetLayout GetNativeLayout() const { return mLayout; }

  static std::shared_ptr<BindGroupLayout> Create(std::vector<GroupEntryInfo> entries, VulkanDevice* device);

 private:
  VulkanDevice* mDevice;
  VkDescriptorSetLayout mLayout;
};

}  // namespace cubic
