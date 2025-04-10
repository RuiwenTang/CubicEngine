#include "render/vk/bind_group_vk.h"

#include <cubic/core/log.h>

#include "render/vk/buffer_vk.h"
#include "render/vk/pipeline_layout_vk.h"
#include "render/vk/texture_vk.h"
#include "render/vk/vulkan_device.h"
#include "render/vk/vulkan_types.h"

namespace cubic {

BindGroupPool::~BindGroupPool() {
  vkResetDescriptorPool(mDevice->GetLogicalDevice(), mPool, 0);

  vkDestroyDescriptorPool(mDevice->GetLogicalDevice(), mPool, nullptr);
}

std::shared_ptr<BindGroupPool> BindGroupPool::Create(VulkanDevice* device) {
  std::vector<VkDescriptorPoolSize> pool_size{};

  pool_size.emplace_back(VkDescriptorPoolSize{
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
      1024,
  });

  pool_size.emplace_back(VkDescriptorPoolSize{
      VK_DESCRIPTOR_TYPE_SAMPLER,
      1024,
  });

  pool_size.emplace_back(VkDescriptorPoolSize{
      VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
      1024,
  });

  VkDescriptorPoolCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  create_info.poolSizeCount = static_cast<uint32_t>(pool_size.size());
  create_info.pPoolSizes = pool_size.data();
  create_info.maxSets = 1024;

  VkDescriptorPool pool;

  if (vkCreateDescriptorPool(device->GetLogicalDevice(), &create_info, nullptr, &pool) != VK_SUCCESS) {
    CUB_ERROR("[Vulkan backend]: failed create new BindGroupPool !!");
    return {};
  }

  return std::make_shared<BindGroupPool>(device, pool);
}

VkDescriptorSet BindGroupPool::Allocate(const PipelineLayoutVK* layout, uint32_t slot, const BindGroup& group) {
  auto set_layout = layout->GetNativeSetLayout(slot);

  if (set_layout == VK_NULL_HANDLE) {
    return VK_NULL_HANDLE;
  }

  VkDescriptorSetAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = mPool;
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &set_layout;

  VkDescriptorSet descriptor_set = VK_NULL_HANDLE;

  if (vkAllocateDescriptorSets(mDevice->GetLogicalDevice(), &alloc_info, &descriptor_set) != VK_SUCCESS) {
    CUB_ERROR("[Vulkan backend]: failed allocate descriptor set !!");
    return VK_NULL_HANDLE;
  }

  std::vector<VkWriteDescriptorSet> write_sets{};
  std::vector<VkDescriptorBufferInfo*> buffer_infos{};
  std::vector<VkDescriptorImageInfo> image_infos{};

  for (const auto& entry : group.GetEntries()) {
    if (entry.type == EntryType::kUniformBuffer) {
      auto buffer_view = std::get<BufferView>(entry.resource);

      buffer_infos.emplace_back(new VkDescriptorBufferInfo);
      buffer_infos.back()->buffer = dynamic_cast<BufferVK*>(buffer_view.buffer.get())->GetNativeBuffer();
      buffer_infos.back()->offset = buffer_view.offset;
      buffer_infos.back()->range = buffer_view.length;

      write_sets.emplace_back();

      auto& set = write_sets.back();
      set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      set.dstSet = descriptor_set;
      set.dstBinding = entry.binding;
      set.descriptorCount = 1;

      set.pBufferInfo = buffer_infos.back();
    } else if (entry.type == EntryType::kTexture) {
    }
  }

  vkUpdateDescriptorSets(mDevice->GetLogicalDevice(), static_cast<uint32_t>(write_sets.size()), write_sets.data(), 0,
                         nullptr);

  for (auto& buffer_info : buffer_infos) {
    delete buffer_info;
  }

  return descriptor_set;
}

}  // namespace cubic
