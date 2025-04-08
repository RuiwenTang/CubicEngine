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

std::shared_ptr<BindGroup> BindGroupPool::Allocate() { return {}; }

}  // namespace cubic
