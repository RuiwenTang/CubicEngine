#include "render/vk/pipeline_layout_vk.h"

#include <cubic/core/log.h>

#include "render/vk/bind_group_vk.h"
#include "render/vk/vulkan_device.h"

namespace cubic {

PipelineLayoutVK::PipelineLayoutVK(std::vector<std::shared_ptr<BindGroupLayout>> groups, VulkanDevice* device,
                                   VkPipelineLayout layout)
    : PipelineLayout(), mDevice(device), mLayout(layout) {}

PipelineLayoutVK::~PipelineLayoutVK() { vkDestroyPipelineLayout(mDevice->GetLogicalDevice(), mLayout, nullptr); }

std::shared_ptr<PipelineLayout> PipelineLayoutVK::Create(std::vector<std::shared_ptr<BindGroupLayout>> groups,
                                                         VulkanDevice* device) {
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.pushConstantRangeCount = 0;
  pipelineLayoutInfo.pPushConstantRanges = nullptr;

  pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(groups.size());

  std::vector<VkDescriptorSetLayout> set_layouts{};

  pipelineLayoutInfo.pSetLayouts = set_layouts.data();

  VkPipelineLayout layout = VK_NULL_HANDLE;

  if (vkCreatePipelineLayout(device->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS) {
    CUB_ERROR("[Vulkan backend] failed create pipeline layout !!");

    return {};
  }

  return std::make_shared<PipelineLayoutVK>(std::move(groups), device, layout);
}

}  // namespace cubic
