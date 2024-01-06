#pragma once

#include <volk.h>

#include <memory>
#include <vector>

namespace cubic {

class VulkanDevice {
 public:
  VulkanDevice() = default;

  ~VulkanDevice();

  static std::unique_ptr<VulkanDevice> Create(VkPhysicalDevice gpu);

  VkDevice GetLogicalDevice() const { return mDevice; }

  VkPhysicalDevice GetPhysicalDevice() const { return mGPUDevice; }

 private:
  bool Init();

  bool InitQueueProps();

  uint32_t GetQueueFamilyIndex(VkQueueFlags flag) const;

 private:
  VkPhysicalDevice mGPUDevice = {};
  VkPhysicalDeviceProperties mGPUProps = {};
  VkPhysicalDeviceFeatures mGPUFeatures = {};
  VkPhysicalDeviceMemoryProperties mGPUMemoryProps = {};

  std::vector<VkQueueFamilyProperties> mGPUQueueProps = {};

  uint32_t mGraphicQueueIndex = 0;
  uint32_t mComputeQueueIndex = 0;
  uint32_t mTransferQueueIndex = 0;

  VkDevice mDevice = {};
};

}  // namespace cubic
