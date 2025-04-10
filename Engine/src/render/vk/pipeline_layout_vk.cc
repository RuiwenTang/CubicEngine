#include "render/vk/pipeline_layout_vk.h"

#include <cubic/core/log.h>

#include "render/vk/bind_group_vk.h"
#include "render/vk/vulkan_device.h"

namespace cubic {

std::vector<VkDescriptorSetLayout> CreateSetLayouts(VulkanDevice* device, const std::vector<BindGroupLayout>& layouts) {
  if (layouts.empty()) {
    return {};
  }

  std::vector<VkDescriptorSetLayout> ret{layouts.size()};

  for (size_t i = 0; i < ret.size(); i++) {
    const auto& layout = layouts[i];

    std::vector<VkDescriptorSetLayoutBinding> bindings{layout.GetEntries().size()};

    for (size_t j = 0; j < bindings.size(); j++) {
      const auto& entry = layout.GetEntries()[j];
      bindings[j].binding = entry.binding;

      bindings[j].descriptorCount = 1;
      bindings[j].pImmutableSamplers = nullptr;

      if (entry.visibility & ShaderStage::kVertexShader) {
        bindings[j].stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
      }

      if (entry.visibility & ShaderStage::kFragmentShader) {
        bindings[j].stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
      }

      if (entry.visibility & ShaderStage::kComputeShader) {
        bindings[j].stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
      }

      switch (entry.type) {
        case EntryType::kUniformBuffer:
          bindings[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
          break;
        case EntryType::kStorgeBuffer:
          bindings[j].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
          break;
        case EntryType::kCombinedImageSampler:
          bindings[j].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
          break;
        case EntryType::kTexture:
          bindings[j].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
          break;
        case EntryType::kSampler:
          bindings[j].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
          break;
      }
    }

    VkDescriptorSetLayoutCreateInfo create_info{VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
    create_info.bindingCount = static_cast<uint32_t>(bindings.size());
    create_info.pBindings = bindings.data();

    auto result = vkCreateDescriptorSetLayout(device->GetLogicalDevice(), &create_info, nullptr, &ret[i]);

    if (result != VK_SUCCESS) {
      CUB_ERROR("[Vulkan backend] failed create descriptor set layout !!");
      return {};
    }
  }

  return ret;
}

PipelineLayoutVK::PipelineLayoutVK(VulkanDevice* device, VkPipelineLayout layout,
                                   std::vector<VkDescriptorSetLayout> setLayouts)
    : PipelineLayout(), mDevice(device), mLayout(layout), mSetLayouts(std::move(setLayouts)) {}

PipelineLayoutVK::~PipelineLayoutVK() {
  vkDestroyPipelineLayout(mDevice->GetLogicalDevice(), mLayout, nullptr);

  for (auto& setLayout : mSetLayouts) {
    vkDestroyDescriptorSetLayout(mDevice->GetLogicalDevice(), setLayout, nullptr);
  }
}

std::unique_ptr<PipelineLayoutVK> PipelineLayoutVK::Create(const std::vector<BindGroupLayout>& groups,
                                                           VulkanDevice* device) {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  std::vector<VkDescriptorSetLayout> set_layouts = CreateSetLayouts(device, groups);

  pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
  pipelineLayoutInfo.pSetLayouts = set_layouts.data();

  VkPipelineLayout layout = VK_NULL_HANDLE;

  if (vkCreatePipelineLayout(device->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS) {
    CUB_ERROR("[Vulkan backend] failed create pipeline layout !!");

    return {};
  }

  return std::make_unique<PipelineLayoutVK>(device, layout, set_layouts);
}

VkDescriptorSetLayout PipelineLayoutVK::GetNativeSetLayout(uint32_t index) const {
  if (index >= mSetLayouts.size()) {
    CUB_ERROR("[Vulkan backend] index out of range !!");
    return VK_NULL_HANDLE;
  }

  return mSetLayouts[index];
}

}  // namespace cubic
