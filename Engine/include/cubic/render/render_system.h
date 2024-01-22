#pragma once

#include <cubic/platform.h>
#include <cubic/render/command_queue.h>
#include <cubic/render/shader_module.h>

#include <memory>

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

  virtual std::shared_ptr<ShaderModule> CreateShaderModule(ShaderModuleDescriptor* desc) = 0;
};

}  // namespace cubic
