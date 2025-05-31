#pragma once

#include <cubic/render/texture.h>
#include <vk_mem_alloc.h>
#include <volk.h>

#include <memory>

namespace cubic {

class VulkanDevice;

struct TextureDescriptorVK {
  VkFormat format = VK_FORMAT_UNDEFINED;
  VkImage image = VK_NULL_HANDLE;
  VkImageView view = VK_NULL_HANDLE;
  VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

  VmaAllocator allocator = VK_NULL_HANDLE;
  VmaAllocation allocation = VK_NULL_HANDLE;

  bool owned = false;
};

class TextureVK : public Texture {
 public:
  enum class Source {
    kSwapchain,
    kUser,
  };

  TextureVK(const TextureDescriptor& desc, const TextureDescriptorVK& vk_desc, VulkanDevice* device, Source source);

  ~TextureVK() override;

  VkImage GetImage() const { return mImage; }

  VkImageView GetImageView() const { return mView; }

  VkImageLayout GetImageLayout() const { return mLayout; }

  Source GetTextureSource() const { return mSource; }

  void SetImageLayout(VkImageLayout layout) { mLayout = layout; }

  static std::shared_ptr<TextureVK> WrapSwapchainTexture(uint32_t width, uint32_t height,
                                                         const TextureDescriptorVK& vk_desc, VulkanDevice* device);

  static std::shared_ptr<Texture> Create(TextureDescriptor* desc, VmaAllocator allocator, VulkanDevice* device);

 private:
  VkImage mImage = VK_NULL_HANDLE;
  VkImageView mView = VK_NULL_HANDLE;
  VkImageLayout mLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  VulkanDevice* mDevice = nullptr;

  Source mSource;

  bool mOwned = false;

  VmaAllocator mAllocator = {};
  VmaAllocation mAllocation = {};
};

}  // namespace cubic
