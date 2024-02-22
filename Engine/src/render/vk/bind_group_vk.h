#pragma once

#include <cubic/render/bind_group.h>
#include <volk.h>

#include <memory>

namespace cubic {

class VulkanDevice;
class PipelineLayoutVK;

class BindGroupLayoutVK : public BindGroupLayout {
 public:
  BindGroupLayoutVK(std::vector<GroupEntryInfo> entries, VulkanDevice* device, VkDescriptorSetLayout layout);

  ~BindGroupLayoutVK() override;

  VkDescriptorSetLayout GetNativeLayout() const { return mLayout; }

  static std::shared_ptr<BindGroupLayout> Create(std::vector<GroupEntryInfo> entries, VulkanDevice* device);

  void SetPipelineLayout(PipelineLayoutVK* layout) { mPipelineLayout = layout; }

  PipelineLayoutVK* GetPipelineLayout() const { return mPipelineLayout; }

 private:
  VulkanDevice* mDevice;
  VkDescriptorSetLayout mLayout;
  PipelineLayoutVK* mPipelineLayout = nullptr;
};

class BindGroupPool : public std::enable_shared_from_this<BindGroupPool> {
 public:
  BindGroupPool(VulkanDevice* device, VkDescriptorPool pool) : mDevice(device), mPool(pool) {}

  ~BindGroupPool();

  static std::shared_ptr<BindGroupPool> Create(VulkanDevice* device);

  std::shared_ptr<BindGroup> Allocate(const std::shared_ptr<BindGroupLayout>& layout, std::vector<GroupEntry> entries);

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
