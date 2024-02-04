#pragma once

#include <cubic/render/render_system.h>
#include <vk_mem_alloc.h>
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

  CommandQueue* GetCommandQueue(QueueType type) override;

  std::shared_ptr<RenderPipeline> CreateRenderPipeline(RenderPipelineDescriptor* desc) override;

  std::shared_ptr<Texture> CreateTexture(TextureDescirptor* desc) override;

  std::shared_ptr<Buffer> CreateBuffer(BufferDescriptor* desc) override;

 protected:
  std::shared_ptr<ShaderModule> CompileBackendShader(ShaderModuleDescriptor* desc,
                                                     const std::vector<uint32_t>& spv) override;

 private:
  bool initInstance(bool enableDebug);

  void setupValidation();

  bool initDevice();

 private:
  VkInstance mInstance = {};

  VkDebugUtilsMessengerEXT mDebugHandler = {};

  std::unique_ptr<VulkanDevice> mDevice = {};

  RenderSystemInfoVK mInfo = {};

  VmaAllocator mAllocator = {};
};

}  // namespace cubic
