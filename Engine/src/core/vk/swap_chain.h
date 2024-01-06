#pragma once

#include <volk.h>

namespace cubic {

class VulkanDevice;

class Swapchain {
 public:
  Swapchain(VkInstance instance, VkSurfaceKHR surface, VulkanDevice* device);

  ~Swapchain();

  bool Init(uint32_t width, uint32_t height, VkSurfaceFormatKHR format);

 private:
  VkInstance mInstance;
  VkSurfaceKHR mSurface;
  VulkanDevice* mDevice;
  VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
};

}  // namespace cubic
