#pragma once

#import <Metal/Metal.h>
#include <cubic/render/command_queue.h>

namespace cubic {

class CommandQueueMTL : public CommandQueue {
 public:
  CommandQueueMTL(id<MTLCommandQueue> queue);

  ~CommandQueueMTL() override;

  std::unique_ptr<CommandBuffer> GenCommandBuffer() override;

  void Submit(std::unique_ptr<CommandBuffer> cmd) override;

 private:
  id<MTLCommandQueue> mQueue;
};

}  // namespace cubic
