#include "render/vk/texture_vk.h"

#include "render/vk/vulkan_device.h"
#include "render/vk/vulkan_types.h"

namespace cubic {

TextureVK::TextureVK(const TextureDescirptor& desc, const TextureDescriptorVK& vk_desc, VulkanDevice* device)
    : Texture(desc),
      mImage(vk_desc.image),
      mView(vk_desc.view),
      mLayout(vk_desc.layout),
      mOwned(vk_desc.owned),
      mDevice(device) {}

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
}

void TextureVK::UploadData(void* data, uint32_t w, uint32_t h, uint32_t x, uint32_t y) {}

std::shared_ptr<TextureVK> TextureVK::WrapSwapchainTexture(uint32_t width, uint32_t height,
                                                         const TextureDescriptorVK& vk_desc, VulkanDevice* device) {
  TextureDescirptor desc{};
  desc.width = width;
  desc.height = height;
  desc.usage = TextureUsage::kRenderTarget;
  desc.format = vk::TypeConvert(vk_desc.format);

  return std::make_shared<TextureVK>(desc, vk_desc, device);
}

}  // namespace cubic
