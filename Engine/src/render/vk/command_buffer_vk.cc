#include "render/vk/command_buffer_vk.h"

namespace cubic {

CommandBufferVK::CommandBufferVK(VkCommandBuffer cmd, uint64_t value) : mCmd(cmd), mSignalValue(value) {}

std::unique_ptr<RenderPass> CommandBufferVK::BeginRenderPass(const RenderPassDescriptor& desc) {
  return std::unique_ptr<RenderPass>();
}

void CommandBufferVK::EndRenderPass(std::unique_ptr<RenderPass> render_pass) {}

}  // namespace cubic
