#pragma once

#include <volk.h>

#include <memory>
#include <vector>

#include "render/vk/texture_vk.h"

namespace cubic {

class VulkanDevice;
class CommandQueueVK;

struct SwapchainBuffer {
  VkImage image;
  VkImageView view;
};

struct SwapchainResult {
  std::shared_ptr<TextureVK> texture;
  VkResult state;

  SwapchainResult(VkResult state) : texture(), state(state) {}

  SwapchainResult(std::shared_ptr<TextureVK> texture, VkResult state) : texture(std::move(texture)), state(state) {}
};

class Swapchain {
 public:
  Swapchain(VkInstance instance, VkSurfaceKHR surface, VulkanDevice* device);

  ~Swapchain();

  bool Resize(uint32_t width, uint32_t height, VkSurfaceFormatKHR format);

  SwapchainResult AcquireNextFrame(VkSemaphore signal_semaphre, VkFence signal_fence);

  VkResult SubmitFrame(CommandQueueVK* queue, std::shared_ptr<TextureVK> texture, VkSemaphore wait_semaphore);

 private:
  void CleanBuffers();

  bool InitBuffers();

 private:
  VkInstance mInstance;
  VkSurfaceKHR mSurface;
  VulkanDevice* mDevice;
  VkSwapchainKHR mSwapchain = VK_NULL_HANDLE;
  VkSurfaceFormatKHR mFormat = {};
  VkExtent2D mSwapchainSize = {};

  std::vector<SwapchainBuffer> mBuffers = {};

  uint32_t mCurrentFrameIndex = UINT32_MAX;
};

}  // namespace cubic
