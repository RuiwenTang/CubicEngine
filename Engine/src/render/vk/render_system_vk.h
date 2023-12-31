#pragma once

#include <cubic/render/render_system.h>
#include <volk.h>

#include <memory>

namespace cubic {

class RenderSystemVk : public RenderSystem {
 public:
  static std::unique_ptr<RenderSystemVk> Create();

  RenderSystemVk() = default;

  ~RenderSystemVk() override;

  Backend GetBackend() const override { return Backend::kVulkan; }

  bool Init(bool enableDebug);

 private:
  bool initInstance(bool enableDebug);

  void setupValidation();

 private:
  VkInstance mInstance = {};

  VkDebugUtilsMessengerEXT mDebugHandler = {};
};

}  // namespace cubic
