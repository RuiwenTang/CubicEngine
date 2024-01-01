#include "render/vk/vulkan_device.h"

#include <cubic/core/log.h>

#include <vector>

namespace cubic {

VulkanDevice::~VulkanDevice() {
  if (mDevice) {
    vkDestroyDevice(mDevice, nullptr);
  }
}

std::unique_ptr<VulkanDevice> VulkanDevice::Create(VkPhysicalDevice gpu) {
  auto device = std::make_unique<VulkanDevice>();

  device->mGPUDevice = gpu;

  vkGetPhysicalDeviceProperties(gpu, &device->mGPUProps);
  vkGetPhysicalDeviceFeatures(gpu, &device->mGPUFeatures);
  vkGetPhysicalDeviceMemoryProperties(gpu, &device->mGPUMemoryProps);

  if (!device->Init()) {
    return std::unique_ptr<VulkanDevice>();
  }

  return device;
}

bool VulkanDevice::Init() {
  if (!InitQueueProps()) {
    return false;
  }

  const float defaultQueuePriority = 0.f;

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos{};

  // graphic queue
  mGraphicQueueIndex = GetQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
  {
    VkDeviceQueueCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    create_info.queueFamilyIndex = mGraphicQueueIndex;
    create_info.queueCount = 1;
    create_info.pQueuePriorities = &defaultQueuePriority;

    queueCreateInfos.emplace_back(create_info);
  }
  mComputeQueueIndex = GetQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
  if (mComputeQueueIndex != mGraphicQueueIndex) {
    VkDeviceQueueCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    create_info.queueFamilyIndex = mComputeQueueIndex;
    create_info.queueCount = 1;
    create_info.pQueuePriorities = &defaultQueuePriority;

    queueCreateInfos.emplace_back(create_info);
  }
  mTransferQueueIndex = GetQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);

  if (mTransferQueueIndex != mGraphicQueueIndex && mTransferQueueIndex != mComputeQueueIndex) {
    VkDeviceQueueCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    create_info.queueFamilyIndex = mComputeQueueIndex;
    create_info.queueCount = 1;
    create_info.pQueuePriorities = &defaultQueuePriority;

    queueCreateInfos.emplace_back(create_info);
  }

  VkDeviceCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  create_info.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  create_info.pQueueCreateInfos = queueCreateInfos.data();
  create_info.pEnabledFeatures = &mGPUFeatures;

  // TODO enable some advance extensions
  std::vector<const char*> requiredExtensions{};

  create_info.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
  create_info.ppEnabledExtensionNames = requiredExtensions.data();

  auto ret = vkCreateDevice(mGPUDevice, &create_info, nullptr, &mDevice);

  if (ret != VK_SUCCESS) {
    CUB_ERROR("Failed create logical vulkan device !!");
    return false;
  }

  volkLoadDevice(mDevice);

  CUB_DEBUG("Success init vulkan context on GPU: {}", mGPUProps.deviceName);

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
