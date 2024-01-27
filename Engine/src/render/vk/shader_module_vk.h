#include <cubic/render/shader_module.h>

#include "render/vk/vulkan_device.h"

namespace cubic {

class ShaderModuleVK : public ShaderModule {
 public:
  ShaderModuleVK(ShaderStage stage, std::string label, VulkanDevice* device, VkShaderModule shader);

  ~ShaderModuleVK() override;

  static std::shared_ptr<ShaderModule> Compile(VulkanDevice* device, const std::vector<uint32_t>& code,
                                               ShaderModuleDescriptor* desc);

 private:
  VulkanDevice* mDevice;
  VkShaderModule mShader;
};

}  // namespace cubic
