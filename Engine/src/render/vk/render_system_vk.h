#pragma once

#include <cubic/render/render_system.h>
#include <volk.h>

#include <memory>

#include "render/render_system_priv.h"
#include "render/vk/render_system_info_vk.h"
#include "render/vk/vulkan_device.h"

namespace cubic {

class RenderSystemVk : public RenderSystemPriv {
 public:
  static std::unique_ptr<RenderSystemVk> Create();

  RenderSystemVk() = default;

  ~RenderSystemVk() override;

  Backend GetBackend() const override { return Backend::kVulkan; }

  bool Init(bool enableDebug);

  RenderSystemInfo* GetBackendInfo() override;

 private:
  bool initInstance(bool enableDebug);

  void setupValidation();

  bool initDevice();

 private:
  VkInstance mInstance = {};

  VkDebugUtilsMessengerEXT mDebugHandler = {};

  std::unique_ptr<VulkanDevice> mDevice = {};

  RenderSystemInfoVK mInfo = {};
};

}  // namespace cubic
