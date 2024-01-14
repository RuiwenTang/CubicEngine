#include "render/vk/command_buffer_vk.h"

namespace cubic {

CommandBufferVK::CommandBufferVK(VkCommandBuffer cmd, uint64_t value) : mCmd(cmd), mSignalValue(value) {}

}  // namespace cubic
