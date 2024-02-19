#include "render/vk/bind_group_vk.h"

#include <cubic/core/log.h>

#include "render/vk/vulkan_device.h"
#include "render/vk/vulkan_types.h"

namespace cubic {

BindGroupLayoutVK::BindGroupLayoutVK(std::vector<GroupEntryInfo> entries, VulkanDevice* device,
                                     VkDescriptorSetLayout layout)
    : BindGroupLayout(std::move(entries)), mDevice(device), mLayout(layout) {}

BindGroupLayoutVK::~BindGroupLayoutVK() { vkDestroyDescriptorSetLayout(mDevice->GetLogicalDevice(), mLayout, nullptr); }

std::shared_ptr<BindGroupLayout> BindGroupLayoutVK::Create(std::vector<GroupEntryInfo> entries, VulkanDevice* device) {
  VkDescriptorSetLayoutCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  create_info.bindingCount = static_cast<uint32_t>(entries.size());

  std::vector<VkDescriptorSetLayoutBinding> bindings{};
  for (const auto& entry : entries) {
    bindings.emplace_back(VkDescriptorSetLayoutBinding{});

    bindings.back().binding = entry.binding;
    bindings.back().descriptorType = vk::TypeConvert(entry.type);
    bindings.back().stageFlags = vk::TypeConvertForBinding(entry.visibility);
    bindings.back().descriptorCount = 1;
  }

  create_info.pBindings = bindings.data();

  VkDescriptorSetLayout layout = VK_NULL_HANDLE;

  if (vkCreateDescriptorSetLayout(device->GetLogicalDevice(), &create_info, nullptr, &layout) != VK_SUCCESS) {
    CUB_ERROR("[Vulkan backend]: Failed create BindGroupLayout !!");
    return {};
  }

  return std::make_shared<BindGroupLayoutVK>(std::move(entries), device, layout);
}

}  // namespace cubic
