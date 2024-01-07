#pragma once

#include <cubic/render/command_buffer.h>
#include <volk.h>

namespace cubic {

class CommandBufferVK : public CommandBuffer {
 public:
  CommandBufferVK(VkCommandBuffer cmd);

  ~CommandBufferVK() override = default;

  VkCommandBuffer GetNativeBuffer() const { return mCmd; }

 private:
  VkCommandBuffer mCmd;
};

}  // namespace cubic
