#pragma once

#include <cubic/platform.h>
#include <cubic/render/command_queue.h>

namespace cubic {

enum class Backend {
  /// Used on Windows, Linux, and Android
  kVulkan,
  /// Used on MacOS and iOS
  kMetal,

  kInvalid,
};

class CUBIC_API RenderSystem {
 public:
  virtual ~RenderSystem() = default;

  virtual Backend GetBackend() const = 0;

  virtual CommandQueue* GetCommandQueue(QueueType type) = 0;
};

}  // namespace cubic
