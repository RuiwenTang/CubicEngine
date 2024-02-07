#pragma once

#include <cubic/render/buffer.h>
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

  void CopyBufferToBuffer(const std::shared_ptr<Buffer>& dst, uint64_t dst_offset, const std::shared_ptr<Buffer>& src,
                          uint64_t src_offset, uint64_t length) override;

  VkCommandBuffer GetNativeBuffer() const { return mCmd; }

  uint64_t GetSignalValue() const { return mSignalValue; }

 private:
  void RecordResource(const std::shared_ptr<Buffer>& buffer);

 private:
  VulkanDevice* mDevice;
  VkCommandBuffer mCmd;
  // signaled value when cmd finished
  uint64_t mSignalValue;
  std::vector<std::shared_ptr<Buffer>> mPendingResources = {};
};

}  // namespace cubic
