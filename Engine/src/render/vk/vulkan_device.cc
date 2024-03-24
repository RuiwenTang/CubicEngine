#include "render/vk/vulkan_device.h"

#include <cubic/core/log.h>

#include <vector>

#include "render/vk/command_queue_vk.h"

namespace cubic {

VulkanDevice::VulkanDevice() = default;

VulkanDevice::~VulkanDevice() {
  vkDeviceWaitIdle(mDevice);

  mGraphicProxy.reset();
  mComputeProxy.reset();
  mTransferProxy.reset();

  if (mDevice) {
    vkDestroyDevice(mDevice, nullptr);
  }
}

std::unique_ptr<VulkanDevice> VulkanDevice::Create(VkPhysicalDevice gpu) {
  auto device = std::make_unique<VulkanDevice>();

  device->mGPUDevice = gpu;

  vkGetPhysicalDeviceProperties(gpu, &device->mGPUProps);
  vkGetPhysicalDeviceFeatures(gpu, &device->mGPUFeatures);

  device->mGPUExtFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
  device->mGPU12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
  device->mGPU13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;

  device->mGPUExtFeatures.pNext = &device->mGPU12Features;
  device->mGPU12Features.pNext = &device->mGPU13Features;
  vkGetPhysicalDeviceFeatures2(gpu, &device->mGPUExtFeatures);

  vkGetPhysicalDeviceMemoryProperties(gpu, &device->mGPUMemoryProps);

  if (!device->Init()) {
    return std::unique_ptr<VulkanDevice>();
  }

  return device;
}

uint32_t VulkanDevice::GetMinBufferAlignment() const { return mGPUProps.limits.minUniformBufferOffsetAlignment; }

bool VulkanDevice::Init() {
  if (!InitQueueProps()) {
    return false;
  }

  const float defaultQueuePriority = 0.f;

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

  // graphic queue
  uint32_t graphicFamily = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
  {
    VkDeviceQueueCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    create_info.queueFamilyIndex = graphicFamily;
    create_info.queueCount = 1;
    create_info.pQueuePriorities = &defaultQueuePriority;

    queueCreateInfos.emplace_back(create_info);

    mGraphicQueueIndex = 0;
  }
  uint32_t computeFamily = GetQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
  if (mComputeQueueIndex != mGraphicQueueIndex) {
    VkDeviceQueueCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    create_info.queueFamilyIndex = computeFamily;
    create_info.queueCount = 1;
    create_info.pQueuePriorities = &defaultQueuePriority;

    queueCreateInfos.emplace_back(create_info);
    mComputeQueueIndex = 1;
  } else {
    mComputeQueueIndex = mGraphicQueueIndex;
  }

  uint32_t transferFamily = GetQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);
  if (mTransferQueueIndex != mGraphicQueueIndex && mTransferQueueIndex != mComputeQueueIndex) {
    VkDeviceQueueCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    create_info.queueFamilyIndex = transferFamily;
    create_info.queueCount = 1;
    create_info.pQueuePriorities = &defaultQueuePriority;

    queueCreateInfos.emplace_back(create_info);
    mTransferQueueIndex = 2;
  } else {
    mTransferQueueIndex = mGraphicQueueIndex;
  }

  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  create_info.pQueueCreateInfos = queueCreateInfos.data();
  create_info.pNext = &mGPUExtFeatures;

  // TODO enable some advance extensions
  std::vector<const char*> requiredExtensions{};

  // swapchain
  requiredExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
  // timeline semaphore
  requiredExtensions.emplace_back(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);

  create_info.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
  create_info.ppEnabledExtensionNames = requiredExtensions.data();

  auto ret = vkCreateDevice(mGPUDevice, &create_info, nullptr, &mDevice);

  if (ret != VK_SUCCESS) {
    CUB_ERROR("Failed create logical vulkan device !!");
    return false;
  }

  volkLoadDevice(mDevice);

  CUB_DEBUG("Success init vulkan context on GPU: {}", mGPUProps.deviceName);

  vkGetDeviceQueue(mDevice, graphicFamily, mGraphicQueueIndex, &mGraphicQueue);
  vkGetDeviceQueue(mDevice, computeFamily, mComputeQueueIndex, &mComputeQueue);
  vkGetDeviceQueue(mDevice, transferFamily, mTransferQueueIndex, &mTransferQueue);

  mGraphicProxy = std::make_unique<CommandQueueVK>(this, mGraphicQueue, graphicFamily);
  mComputeProxy = std::make_unique<CommandQueueVK>(this, mComputeQueue, computeFamily);
  mTransferProxy = std::make_unique<CommandQueueVK>(this, mTransferQueue, transferFamily);

  if (!mGraphicProxy->Init()) {
    return false;
  }

  if (!mComputeProxy->Init()) {
    return false;
  }

  if (!mTransferProxy->Init()) {
    return false;
  }

  return true;
}

bool VulkanDevice::InitQueueProps() {
  uint32_t queueCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(mGPUDevice, &queueCount, nullptr);

  if (queueCount == 0) {
    return false;
  }

  mGPUQueueProps.resize(queueCount);

  vkGetPhysicalDeviceQueueFamilyProperties(mGPUDevice, &queueCount, mGPUQueueProps.data());

  return true;
}

uint32_t VulkanDevice::GetQueueFamilyIndex(VkQueueFlags flag) const {
  for (uint32_t i = 0; i < static_cast<uint32_t>(mGPUQueueProps.size()); i++) {
    if (mGPUQueueProps[i].queueFlags & flag) {
      return i;
    }
  }

  CUB_ERROR("Can not find queue which support [ {} ]flag", flag);

  return 0;
}

}  // namespace cubic
