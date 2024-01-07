#pragma once

#include <cubic/platform.h>

#include <memory>

namespace cubic {

enum class QueueType {
  kGraphic,
  kCompute,
  kTransfer,
};

class CommandBuffer;

class CUBIC_API CommandQueue {
 public:
  virtual ~CommandQueue() = default;

  virtual std::unique_ptr<CommandBuffer> GenCommandBuffer() = 0;

  virtual void Submit(std::unique_ptr<CommandBuffer> cmd) = 0;
};

}  // namespace cubic
