#pragma once

#include <cubic/render/bind_group.h>
#include <volk.h>

#include <memory>

namespace cubic {

class VulkanDevice;
class PipelineLayoutVK;

class BindGroupPool : public std::enable_shared_from_this<BindGroupPool> {
 public:
  BindGroupPool(VulkanDevice* device, VkDescriptorPool pool) : mDevice(device), mPool(pool) {}

  ~BindGroupPool();

  static std::shared_ptr<BindGroupPool> Create(VulkanDevice* device);

  std::shared_ptr<BindGroup> Allocate();

 private:
  VulkanDevice* mDevice;
  VkDescriptorPool mPool;
};

class BindGroupVK : public BindGroup {
 public:
  BindGroupVK(VkDescriptorSet desccriptorSet, VkPipelineLayout pipelineLayout, std::shared_ptr<BindGroupPool> pool)
      : mSet(desccriptorSet), mPipelineLayout(pipelineLayout), mPool(std::move(pool)) {}

  ~BindGroupVK() override = default;

  VkDescriptorSet GetNativeSet() const { return mSet; }

  VkPipelineLayout GetPipelineLayout() const { return mPipelineLayout; }

 private:
  VkDescriptorSet mSet;
  VkPipelineLayout mPipelineLayout;
  std::shared_ptr<BindGroupPool> mPool;
};

}  // namespace cubic
