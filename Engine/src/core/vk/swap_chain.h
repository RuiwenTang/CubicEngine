#pragma once

#include <volk.h>

#include <vector>

namespace cubic {

class VulkanDevice;

struct SwapchainBuffer {
  VkImage image;
  VkImageView view;
};

class Swapchain {
 public:
  Swapchain(VkInstance instance, VkSurfaceKHR surface, VulkanDevice* device);

  ~Swapchain();

  bool Resize(uint32_t width, uint32_t height, VkSurfaceFormatKHR format);

 private:
  void CleanBuffers();

  bool InitBuffers();

 private:
  VkInstance mInstance;
  VkSurfaceKHR mSurface;
  VulkanDevice* mDevice;
  VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
  VkSurfaceFormatKHR mFormat = {};

  std::vector<SwapchainBuffer> mBuffers = {};

  uint32_t mCurrentFrameIndex = UINT32_MAX;
};

}  // namespace cubic
