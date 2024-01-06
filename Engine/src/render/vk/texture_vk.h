#pragma once

#include <cubic/render/texture.h>
#include <volk.h>

namespace cubic {

class VulkanDevice;

struct TextureDescriptorVK {
  VkImage image = VK_NULL_HANDLE;
  VkImageView view = VK_NULL_HANDLE;
  VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;

  bool owned = false;
};

class TextureVK : public Texture {
 public:
  TextureVK(const TextureDescirptor& desc, const TextureDescriptorVK& vk_desc, VulkanDevice* device);

  ~TextureVK() override;

  void UploadData(void* data, uint32_t w, uint32_t h, uint32_t x, uint32_t y) override;

  VkImage GetImage() const { return mImage; }

  VkImageView GetImageView() const { return mView; }

  VkImageLayout GetImageLayout() const { return mLayout; }

 private:
  VkImage mImage = VK_NULL_HANDLE;
  VkImageView mView = VK_NULL_HANDLE;
  VkImageLayout mLayout = VK_IMAGE_LAYOUT_UNDEFINED;

  VulkanDevice* mDevice = nullptr;

  bool mOwned = false;
};

}  // namespace cubic
