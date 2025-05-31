#include "render/vk/texture_vk.h"

#include <cubic/core/log.h>

#include "render/vk/vulkan_device.h"
#include "render/vk/vulkan_types.h"

namespace cubic {

TextureVK::TextureVK(const TextureDescriptor& desc, const TextureDescriptorVK& vk_desc, VulkanDevice* device,
                     Source source)
    : Texture(desc),
      mImage(vk_desc.image),
      mView(vk_desc.view),
      mLayout(vk_desc.layout),
      mOwned(vk_desc.owned),
      mDevice(device),
      mSource(source),
      mAllocator(vk_desc.allocator),
      mAllocation(vk_desc.allocation) {}

TextureVK::~TextureVK() {
  if (!mOwned) {
    return;
  }

  if (mView) {
    vkDestroyImageView(mDevice->GetLogicalDevice(), mView, nullptr);
  }

  if (mImage) {
    vkDestroyImage(mDevice->GetLogicalDevice(), mImage, nullptr);
  }

  if (mAllocation) {
    vmaFreeMemory(mAllocator, mAllocation);
  }
}

std::shared_ptr<TextureVK> TextureVK::WrapSwapchainTexture(uint32_t width, uint32_t height,
                                                           const TextureDescriptorVK& vk_desc, VulkanDevice* device) {
  TextureDescriptor desc{};
  desc.width = width;
  desc.height = height;
  desc.usage = TextureUsage::kRenderTarget;
  desc.format = vk::TypeConvert(vk_desc.format);

  return std::make_shared<TextureVK>(desc, vk_desc, device, TextureVK::Source::kSwapchain);
}

std::shared_ptr<Texture> TextureVK::Create(TextureDescriptor* desc, VmaAllocator allocator, VulkanDevice* device) {
  VkImageCreateInfo image_info{};
  image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_info.extent.width = desc->width;
  image_info.extent.height = desc->height;
  image_info.extent.depth = 1;
  image_info.imageType = VK_IMAGE_TYPE_2D;
  image_info.mipLevels = 1;
  image_info.arrayLayers = 1;
  image_info.tiling = VK_IMAGE_TILING_OPTIMAL;

  image_info.format = vk::TypeConvert(desc->format);
  image_info.usage = vk::TypeConvert(desc->usage, desc->format);
  image_info.samples = vk::TypeConvert(desc->sample_count);

  VmaAllocationCreateInfo allocation_info{};
  allocation_info.usage = VMA_MEMORY_USAGE_AUTO;
  allocation_info.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

  if (desc->usage & TextureUsage::kRenderTarget && desc->sample_count > 1) {
    allocation_info.preferredFlags |= VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
  }

  VkImage image = VK_NULL_HANDLE;
  VmaAllocation allocation = VK_NULL_HANDLE;
  if (vmaCreateImage(allocator, &image_info, &allocation_info, &image, &allocation, nullptr) != VK_SUCCESS) {
    CUB_ERROR("[Vulkan backend] failed create image with size: [ {}, {} ] !!", desc->width, desc->height);
    return {};
  }

  VkImageViewCreateInfo view_info{};
  view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  view_info.format = image_info.format;
  view_info.image = image;
  view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
  if (desc->format == TextureFormat::kStencil8) {
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT;
  } else if (desc->format == TextureFormat::kDepth24) {
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
  } else if (desc->format == TextureFormat::kDepth24Stencil8) {
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_STENCIL_BIT | VK_IMAGE_ASPECT_DEPTH_BIT;
  } else {
    view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  }
  view_info.subresourceRange.baseMipLevel = 0;
  view_info.subresourceRange.levelCount = 1;
  view_info.subresourceRange.baseArrayLayer = 0;
  view_info.subresourceRange.layerCount = 1;

  VkImageView view = VK_NULL_HANDLE;

  if (vkCreateImageView(device->GetLogicalDevice(), &view_info, nullptr, &view) != VK_SUCCESS) {
    CUB_ERROR("[Vulkan backend] failed create image view with size: [ {}, {} ] !!", desc->width, desc->height);
    vkDestroyImage(device->GetLogicalDevice(), image, nullptr);

    vmaFreeMemory(allocator, allocation);
    return {};
  }

  TextureDescriptorVK desc_vk{};
  desc_vk.format = image_info.format;
  desc_vk.layout = VK_IMAGE_LAYOUT_UNDEFINED;
  desc_vk.image = image;
  desc_vk.view = view;
  desc_vk.owned = true;

  desc_vk.allocator = allocator;
  desc_vk.allocation = allocation;

  return std::make_shared<TextureVK>(*desc, desc_vk, device, Source::kUser);
}

}  // namespace cubic
