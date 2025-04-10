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

  VkDescriptorSet Allocate(const PipelineLayoutVK* layout, uint32_t slot, const BindGroup& group);

 private:
  VulkanDevice* mDevice;
  VkDescriptorPool mPool;
};

}  // namespace cubic
