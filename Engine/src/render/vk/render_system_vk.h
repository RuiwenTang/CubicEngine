#pragma once

#include <cubic/render/render_system.h>
#include <volk.h>

#include <memory>

#include "render/vk/vulkan_device.h"

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

  bool initDevice();

 private:
  VkInstance mInstance = {};

  VkDebugUtilsMessengerEXT mDebugHandler = {};

  std::unique_ptr<VulkanDevice> mDevice = {};
};

}  // namespace cubic
