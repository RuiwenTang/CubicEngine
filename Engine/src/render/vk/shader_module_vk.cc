#include "render/vk/shader_module_vk.h"

#include <cubic/core/log.h>

namespace cubic {

ShaderModuleVK::ShaderModuleVK(ShaderStage stage, std::string label, VulkanDevice* device, VkShaderModule shader)
    : ShaderModule(stage, label), mDevice(device), mShader(shader) {}

ShaderModuleVK::~ShaderModuleVK() { vkDestroyShaderModule(mDevice->GetLogicalDevice(), mShader, nullptr); }

std::shared_ptr<ShaderModule> ShaderModuleVK::Compile(VulkanDevice* device, const std::vector<uint32_t>& code,
                                                      ShaderModuleDescriptor* desc) {
  VkShaderModuleCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  create_info.codeSize = code.size() * sizeof(uint32_t);
  create_info.pCode = code.data();

  VkShaderModule module = VK_NULL_HANDLE;

  if (vkCreateShaderModule(device->GetLogicalDevice(), &create_info, nullptr, &module) != VK_SUCCESS) {
    CUB_ERROR("[Vulkan backend] failed compile shader for : {}", desc->label);

    return {};
  }

  return std::make_shared<ShaderModuleVK>(desc->stage, desc->label, device, module);
}

}  // namespace cubic
