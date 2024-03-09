#include "render/vk/command_queue_vk.h"

#include "render/vk/command_buffer_vk.h"
#include "render/vk/vulkan_device.h"

namespace cubic {

CommandQueueVK::CommandQueueVK(VulkanDevice* device, VkQueue queue, uint32_t queueFamily)
    : mDevice(device), mQueue(queue), mQueueFamily(queueFamily) {}

CommandQueueVK::~CommandQueueVK() {
  vkQueueWaitIdle(mQueue);

  if (mPool) {
    vkDestroyCommandPool(mDevice->GetLogicalDevice(), mPool, nullptr);
  }

  if (mTimelineSemaphore) {
    vkDestroySemaphore(mDevice->GetLogicalDevice(), mTimelineSemaphore, nullptr);
  }
}

std::unique_ptr<CommandBuffer> CommandQueueVK::GenCommandBuffer() {
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandPool = mPool;
  alloc_info.commandBufferCount = 1;

  VkCommandBuffer cmd = VK_NULL_HANDLE;

  auto ret = vkAllocateCommandBuffers(mDevice->GetLogicalDevice(), &alloc_info, &cmd);

  if (ret != VK_SUCCESS) {
    return std::unique_ptr<CommandBuffer>();
  }

  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  ret = vkBeginCommandBuffer(cmd, &begin_info);
  if (ret != VK_SUCCESS) {
    return std::unique_ptr<CommandBuffer>();
  }

  mCmdID++;

  return std::make_unique<CommandBufferVK>(mDevice, cmd, mCmdID);
}

void CommandQueueVK::Submit(std::unique_ptr<CommandBuffer> cmd) {
  auto vk_cmd = dynamic_cast<CommandBufferVK*>(cmd.get());

  if (vk_cmd == nullptr) {
    return;
  }

  auto ret = vkEndCommandBuffer(vk_cmd->GetNativeBuffer());

  if (ret != VK_SUCCESS) {
    return;
  }

  VkCommandBuffer native_cmd = vk_cmd->GetNativeBuffer();

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &native_cmd;

  VkTimelineSemaphoreSubmitInfo timeline_info{};
  timeline_info.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
  timeline_info.signalSemaphoreValueCount = 1;
  auto value = vk_cmd->GetSignalValue();
  timeline_info.pSignalSemaphoreValues = &value;

  submit_info.pNext = &timeline_info;
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &mTimelineSemaphore;

  vkQueueSubmit(mQueue, 1, &submit_info, VK_NULL_HANDLE);

  mPendingCMD.emplace_back(std::move(cmd));
}

bool CommandQueueVK::Init() {
  VkCommandPoolCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  create_info.queueFamilyIndex = mQueueFamily;
  create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

  if (vkCreateCommandPool(mDevice->GetLogicalDevice(), &create_info, nullptr, &mPool) != VK_SUCCESS) {
    return false;
  }

  VkSemaphoreTypeCreateInfo timeline_create_info{};
  timeline_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
  timeline_create_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
  timeline_create_info.initialValue = 0;

  VkSemaphoreCreateInfo semaphore_create_info{};
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  semaphore_create_info.pNext = &timeline_create_info;
  semaphore_create_info.flags = 0;

  if (vkCreateSemaphore(mDevice->GetLogicalDevice(), &semaphore_create_info, nullptr, &mTimelineSemaphore) !=
      VK_SUCCESS) {
    return false;
  }

  return true;
}

void CommandQueueVK::ResetPool() {
  if (mPendingCMD.empty()) {
    return;
  }

  auto value = dynamic_cast<CommandBufferVK*>(mPendingCMD.back().get())->GetSignalValue();

  VkSemaphoreWaitInfo wait_info{};
  wait_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
  wait_info.semaphoreCount = 1;
  wait_info.pSemaphores = &mTimelineSemaphore;
  wait_info.pValues = &value;

  vkWaitSemaphores(mDevice->GetLogicalDevice(), &wait_info, UINT16_MAX);

  vkResetCommandPool(mDevice->GetLogicalDevice(), mPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

  std::vector<VkCommandBuffer> vk_cmds{};
  for (auto const& cmd : mPendingCMD) {
    vk_cmds.emplace_back(dynamic_cast<CommandBufferVK*>(cmd.get())->GetNativeBuffer());
  }

  vkFreeCommandBuffers(mDevice->GetLogicalDevice(), mPool, vk_cmds.size(), vk_cmds.data());

  mPendingCMD.clear();
}

}  // namespace cubic
