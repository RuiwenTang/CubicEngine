#pragma once

#include <cubic/render/command_buffer.h>
#include <volk.h>

#include <memory>
#include <vector>

namespace cubic {

class VulkanDevice;

class CommandBufferVK : public CommandBuffer {
 public:
  CommandBufferVK(VulkanDevice* device, VkCommandBuffer cmd, uint64_t value);

  ~CommandBufferVK() override;

  std::unique_ptr<RenderPass> BeginRenderPass(const RenderPassDescriptor& desc) override;

  void EndRenderPass(std::unique_ptr<RenderPass> render_pass) override;

  VkCommandBuffer GetNativeBuffer() const { return mCmd; }

  uint64_t GetSignalValue() const { return mSignalValue; }

 private:
  VulkanDevice* mDevice;
  VkCommandBuffer mCmd;
  // signaled value when cmd finished
  uint64_t mSignalValue;

  std::vector<std::unique_ptr<RenderPass>> mPendingRenderPass = {};
};

}  // namespace cubic
