#include "render/vk/command_queue_vk.h"

#include "render/vk/command_buffer_vk.h"
#include "render/vk/vulkan_device.h"

namespace cubic {

CommandQueueVK::CommandQueueVK(VulkanDevice* device, VkQueue queue, uint32_t queueFamily)
    : mDevice(device), mQueue(queue), mQueueFamily(queueFamily) {}

CommandQueueVK::~CommandQueueVK() {
  if (mPool) {
    vkDestroyCommandPool(mDevice->GetLogicalDevice(), mPool, nullptr);
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

  return std::make_unique<CommandBufferVK>(cmd);
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

  vkQueueSubmit(mQueue, 1, &submit_info, VK_NULL_HANDLE);
}

bool CommandQueueVK::Init() {
  VkCommandPoolCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  create_info.queueFamilyIndex = mQueueFamily;
  create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

  return vkCreateCommandPool(mDevice->GetLogicalDevice(), &create_info, nullptr, &mPool) == VK_SUCCESS;
}

void CommandQueueVK::ResetPool() {
  vkQueueWaitIdle(mQueue);
  vkResetCommandPool(mDevice->GetLogicalDevice(), mPool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
}

}  // namespace cubic
