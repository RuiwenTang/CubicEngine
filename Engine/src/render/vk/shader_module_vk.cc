#include "render/vk/shader_module_vk.h"

#include <cubic/core/log.h>
#include <spirv-reflect/spirv_reflect.h>

namespace cubic {

std::vector<BindGroupLayout> ReflectShader(ShaderStageMask stages, const std::vector<uint32_t>& code) {
  SpvReflectShaderModule module = {};
  auto result = spvReflectCreateShaderModule(code.size() * sizeof(uint32_t), code.data(), &module);

  if (result != SPV_REFLECT_RESULT_SUCCESS) {
    CUB_ERROR("[Vulkan backend] failed to reflect shader module: {}", static_cast<uint32_t>(result));
    return {};
  }

  std::vector<BindGroupLayout> layouts;

  for (uint32_t i = 0; i < module.descriptor_set_count; ++i) {
    const SpvReflectDescriptorSet* set = &module.descriptor_sets[i];

    BindGroupLayout layout(set->set);
    for (uint32_t j = 0; j < set->binding_count; ++j) {
      const SpvReflectDescriptorBinding* binding = set->bindings[j];

      EntryType type = EntryType::kNone;

      switch (binding->descriptor_type) {
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER:
          type = EntryType::kSampler;
          break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
          type = EntryType::kCombinedImageSampler;
          break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
          type = EntryType::kUniformBuffer;
          break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_BUFFER:
          type = EntryType::kStorgeBuffer;
          break;
        case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
          type = EntryType::kTexture;
          break;
        default:
          CUB_ERROR("[Vulkan backend] unknown descriptor type ");
          break;
      }

      layout.AddBinding(binding->binding, type, stages);
    }
    layouts.push_back(layout);
  }
  spvReflectDestroyShaderModule(&module);
  return layouts;
}

ShaderModuleVK::ShaderModuleVK(ShaderStage stage, std::string label, std::vector<BindGroupLayout> groups,
                               VulkanDevice* device, VkShaderModule shader)
    : ShaderModule(stage, label, std::move(groups)), mDevice(device), mShader(shader) {}

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

  return std::make_shared<ShaderModuleVK>(desc->stage, desc->label, ReflectShader(desc->stage, code), device, module);
}

}  // namespace cubic
