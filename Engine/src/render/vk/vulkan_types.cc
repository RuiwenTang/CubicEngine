#include "render/vk/vulkan_types.h"

namespace cubic::vk {

TextureFormat TypeConvert(VkFormat format) {
  switch (format) {
    case VK_FORMAT_B8G8R8A8_UNORM:
      return TextureFormat::kBGRA8Unorm;
    case VK_FORMAT_R8G8B8A8_UNORM:
      return TextureFormat::kRGBA8Unorm;
    case VK_FORMAT_R8G8B8_UNORM:
      return TextureFormat::kRGB8Unorm;
    case VK_FORMAT_R8_UNORM:
      return TextureFormat::kR8Unorm;
  }

  return TextureFormat::kInvalid;
}

VkFormat TypeConvert(TextureFormat format) {
  switch (format) {
    case TextureFormat::kInvalid:
      return VK_FORMAT_UNDEFINED;
    case TextureFormat::kR8Unorm:
      return VK_FORMAT_R8_UNORM;
    case TextureFormat::kRGB8Unorm:
      return VK_FORMAT_R8G8B8_UNORM;
    case TextureFormat::kRGBA8Unorm:
      return VK_FORMAT_R8G8B8A8_UNORM;
    case TextureFormat::kBGRA8Unorm:
      return VK_FORMAT_B8G8R8A8_UNORM;
    case TextureFormat::kStencil8:
      return VK_FORMAT_S8_UINT;
    case TextureFormat::kDepth24:
      return VK_FORMAT_D32_SFLOAT;
    case TextureFormat::kDepth24Stencil8:
      return VK_FORMAT_D24_UNORM_S8_UINT;
  }

  return VK_FORMAT_UNDEFINED;
}

VkAttachmentLoadOp TypeConvert(LoadOp op) {
  switch (op) {
    case LoadOp::kUndefined:
      return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    case LoadOp::kClear:
      return VK_ATTACHMENT_LOAD_OP_CLEAR;
    case LoadOp::kLoad:
      return VK_ATTACHMENT_LOAD_OP_LOAD;
  }

  return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
}

VkAttachmentStoreOp TypeConvert(StoreOp op) {
  switch (op) {
    case StoreOp::kUndefined:
    case StoreOp::kDiscard:
      return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    case StoreOp::kStore:
    case StoreOp::kMSAAResolve:
      return VK_ATTACHMENT_STORE_OP_STORE;
  }

  return VK_ATTACHMENT_STORE_OP_DONT_CARE;
}

VkSampleCountFlagBits TypeConvert(uint32_t count) {
  if (count == 1) {
    return VK_SAMPLE_COUNT_1_BIT;
  } else if (count == 4) {
    return VK_SAMPLE_COUNT_4_BIT;
  } else if (count == 8) {
    return VK_SAMPLE_COUNT_8_BIT;
  } else if (count == 16) {
    return VK_SAMPLE_COUNT_16_BIT;
  } else if (count == 32) {
    return VK_SAMPLE_COUNT_32_BIT;
  } else if (count == 64) {
    return VK_SAMPLE_COUNT_64_BIT;
  }

  return VK_SAMPLE_COUNT_1_BIT;
}

VkClearValue TypeConvert(const GPUColor& color) {
  VkClearValue ret{};

  ret.color = {color.r, color.g, color.b, color.a};

  return ret;
}

VkClearValue TypeConvert(float depth, uint32_t stencil) {
  VkClearValue ret{};

  ret.depthStencil = {depth, stencil};

  return ret;
}

}  // namespace cubic::vk
