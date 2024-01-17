#include "render/mtl/command_queue_mtl.h"
#include "render/mtl/command_buffer_mtl.h"

namespace cubic {

CommandQueueMTL::CommandQueueMTL(id<MTLCommandQueue> queue) : mQueue(queue) {}

CommandQueueMTL::~CommandQueueMTL() { [mQueue release]; }

std::unique_ptr<CommandBuffer> CommandQueueMTL::GenCommandBuffer() {
  id<MTLCommandBuffer> cmd = [mQueue commandBuffer];

  if (cmd == nil) {
    return {};
  }

  return std::make_unique<CommandBufferMTL>(cmd);
}

void CommandQueueMTL::Submit(std::unique_ptr<CommandBuffer> cmd) {
  auto cmd_mtl = dynamic_cast<CommandBufferMTL*>(cmd.get());

  if (cmd_mtl == nullptr) {
    return;
  }

  [cmd_mtl->GetNativeCMD() commit];
}

}
