#include "render/vk/bind_group_vk.h"

#include <cubic/core/log.h>

#include "render/vk/buffer_vk.h"
#include "render/vk/pipeline_layout_vk.h"
#include "render/vk/texture_vk.h"
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

std::shared_ptr<BindGroup> BindGroupPool::Allocate(const std::shared_ptr<BindGroupLayout>& layout,
                                                   std::vector<GroupEntry> entries) {
  auto layout_vk = dynamic_cast<BindGroupLayoutVK*>(layout.get());

  if (layout_vk == nullptr) {
    return {};
  }

  auto native_layout = layout_vk->GetNativeLayout();

  VkDescriptorSetAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.descriptorPool = mPool;
  alloc_info.descriptorSetCount = 1;
  alloc_info.pSetLayouts = &native_layout;

  VkDescriptorSet descriptor_set = VK_NULL_HANDLE;

  if (vkAllocateDescriptorSets(mDevice->GetLogicalDevice(), &alloc_info, &descriptor_set) != VK_SUCCESS) {
    return {};
  }

  std::vector<std::pair<uint32_t, VkDescriptorBufferInfo>> buff_info{};
  std::vector<std::pair<uint32_t, VkDescriptorImageInfo>> img_info{};
  std::vector<VkWriteDescriptorSet> write_info{};

  for (auto const& entry : entries) {
    if (entry.type == EntryType::kUniformBuffer) {
      buff_info.emplace_back(std::make_pair(entry.binding, VkDescriptorBufferInfo{}));

      auto buffer_view = std::get<BufferView>(entry.resource);

      buff_info.back().second.buffer = dynamic_cast<BufferVK*>(buffer_view.buffer.get())->GetNativeBuffer();
      buff_info.back().second.offset = buffer_view.offset;
      buff_info.back().second.range = buffer_view.length;
    } else if (entry.type == EntryType::kTexture) {
      img_info.emplace_back(std::make_pair(entry.binding, VkDescriptorImageInfo{}));

      auto texture = std::get<std::shared_ptr<Texture>>(entry.resource);

      auto texture_vk = dynamic_cast<TextureVK*>(texture.get());

      img_info.back().second.imageView = texture_vk->GetImageView();
      img_info.back().second.imageLayout = texture_vk->GetImageLayout();
    }
  }

  for (size_t i = 0; i < buff_info.size(); i++) {
    write_info.emplace_back();

    write_info.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_info.back().dstSet = descriptor_set;
    write_info.back().dstBinding = buff_info[i].first;
    write_info.back().descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write_info.back().descriptorCount = 1;
    write_info.back().pBufferInfo = &buff_info[i].second;
  }

  for (size_t i = 0; i < img_info.size(); i++) {
    write_info.emplace_back();

    write_info.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_info.back().dstSet = descriptor_set;
    write_info.back().dstBinding = img_info[i].first;
    write_info.back().descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    write_info.back().descriptorCount = 1;
    write_info.back().pImageInfo = &img_info[i].second;
  }

  vkUpdateDescriptorSets(mDevice->GetLogicalDevice(), static_cast<uint32_t>(write_info.size()), write_info.data(), 0,
                         nullptr);

  return std::make_shared<BindGroupVK>(descriptor_set, layout_vk->GetPipelineLayout()->GetNativeLayout(),
                                       shared_from_this());
}

}  // namespace cubic
