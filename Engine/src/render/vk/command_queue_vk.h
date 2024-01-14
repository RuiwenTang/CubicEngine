#pragma once

#include <cubic/render/command_queue.h>
#include <volk.h>

#include <vector>

namespace cubic {

class VulkanDevice;

class CommandQueueVK : public CommandQueue {
 public:
  CommandQueueVK(VulkanDevice* device, VkQueue queue, uint32_t queueFamily);

  ~CommandQueueVK() override;

  std::unique_ptr<CommandBuffer> GenCommandBuffer() override;

  void Submit(std::unique_ptr<CommandBuffer> cmd) override;

  bool Init();

  VkQueue GetNativeQueue() const { return mQueue; }

  void ResetPool();

 private:
  VulkanDevice* mDevice;
  VkQueue mQueue;
  uint32_t mQueueFamily;

  VkCommandPool mPool = VK_NULL_HANDLE;
  VkSemaphore mTimelineSemaphore = VK_NULL_HANDLE;

  uint64_t mCmdID = 0;

  std::vector<VkCommandBuffer> mPendingCMD = {};
};

}  // namespace cubic
