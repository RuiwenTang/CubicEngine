#pragma once

#include <cubic/render/command_buffer.h>
#include <volk.h>

namespace cubic {

class CommandBufferVK : public CommandBuffer {
 public:
  CommandBufferVK(VkCommandBuffer cmd, uint64_t value);

  ~CommandBufferVK() override = default;

  VkCommandBuffer GetNativeBuffer() const { return mCmd; }

  uint64_t GetSignalValue() const { return mSignalValue; }

 private:
  VkCommandBuffer mCmd;
  // signaled value when cmd finished
  uint64_t mSignalValue;
};

}  // namespace cubic
