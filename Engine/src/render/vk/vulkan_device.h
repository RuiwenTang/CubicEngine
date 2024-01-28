#pragma once

#include <volk.h>

#include <memory>
#include <vector>

namespace cubic {

class CommandQueueVK;

class VulkanDevice {
 public:
  VulkanDevice() = default;

  ~VulkanDevice();

  static std::unique_ptr<VulkanDevice> Create(VkPhysicalDevice gpu);

  VkDevice GetLogicalDevice() const { return mDevice; }

  VkPhysicalDevice GetPhysicalDevice() const { return mGPUDevice; }

  CommandQueueVK* GetGraphicQueue() const { return mGraphicProxy.get(); }

  CommandQueueVK* GetComputeQueue() const { return mComputeProxy.get(); }

  CommandQueueVK* GetTransferQueue() const { return mTransferProxy.get(); }

 private:
  bool Init();

  bool InitQueueProps();

  uint32_t GetQueueFamilyIndex(VkQueueFlags flag) const;

 private:
  VkPhysicalDevice mGPUDevice = {};
  VkPhysicalDeviceProperties mGPUProps = {};
  VkPhysicalDeviceFeatures mGPUFeatures = {};
  VkPhysicalDeviceFeatures2 mGPUExtFeatures = {};
  VkPhysicalDeviceVulkan12Features mGPU12Features = {};
  VkPhysicalDeviceVulkan13Features mGPU13Features = {};
  VkPhysicalDeviceMemoryProperties mGPUMemoryProps = {};

  std::vector<VkQueueFamilyProperties> mGPUQueueProps = {};

  uint32_t mGraphicQueueIndex = 0;
  uint32_t mComputeQueueIndex = 0;
  uint32_t mTransferQueueIndex = 0;

  VkDevice mDevice = {};

  VkQueue mGraphicQueue = VK_NULL_HANDLE;
  VkQueue mComputeQueue = VK_NULL_HANDLE;
  VkQueue mTransferQueue = VK_NULL_HANDLE;

  std::unique_ptr<CommandQueueVK> mGraphicProxy = {};
  std::unique_ptr<CommandQueueVK> mComputeProxy = {};
  std::unique_ptr<CommandQueueVK> mTransferProxy = {};
};

}  // namespace cubic
