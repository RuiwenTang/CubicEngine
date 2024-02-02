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

VkColorComponentFlags TypeConvert(ColorWriteMask mask) {
  VkColorComponentFlags ret = 0;

  if (mask & ColorWriteMask::kRed) {
    ret |= VK_COLOR_COMPONENT_R_BIT;
  }

  if (mask & ColorWriteMask::kGreen) {
    ret |= VK_COLOR_COMPONENT_G_BIT;
  }

  if (mask & ColorWriteMask::kBlue) {
    ret |= VK_COLOR_COMPONENT_B_BIT;
  }

  if (mask & ColorWriteMask::kAlpha) {
    ret |= VK_COLOR_COMPONENT_A_BIT;
  }

  return ret;
}

VkBlendFactor TypeConvert(BlendFactor factor) {
  switch (factor) {
    case BlendFactor::kZero:
      return VK_BLEND_FACTOR_ZERO;
    case BlendFactor::kOne:
      return VK_BLEND_FACTOR_ONE;
    case BlendFactor::kSrc:
      return VK_BLEND_FACTOR_SRC_COLOR;
    case BlendFactor::kOneMinusSrc:
      return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case BlendFactor::kSrcAlpha:
      return VK_BLEND_FACTOR_SRC_ALPHA;
    case BlendFactor::kOneMinusSrcAlpha:
      return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::kDst:
      return VK_BLEND_FACTOR_DST_COLOR;
    case BlendFactor::kOneMinusDst:
      return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case BlendFactor::kDstAlpha:
      return VK_BLEND_FACTOR_DST_ALPHA;
    case BlendFactor::kOneMinusDstAlpha:
      return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
  }
}

VkBlendOp TypeConvert(BlendOp op) {
  switch (op) {
    case BlendOp::kAdd:
      return VK_BLEND_OP_ADD;
    case BlendOp::kSubtract:
      return VK_BLEND_OP_SUBTRACT;
    case BlendOp::kReverseSubtract:
      return VK_BLEND_OP_REVERSE_SUBTRACT;
    case BlendOp::kMin:
      return VK_BLEND_OP_MIN;
    case BlendOp::kMax:
      return VK_BLEND_OP_MAX;
  }
}

VkImageUsageFlagBits TypeConvert(TextureUsageMask mask, TextureFormat format) {
  VkFlags flags = {};

  if (mask & TextureUsage::kCopySrc) {
    flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }

  if (mask & TextureUsage::kCopyDst) {
    flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }

  if (mask & TextureUsage::kRenderTarget) {
    if (format == TextureFormat::kStencil8 || format == TextureFormat::kDepth24 ||
        format == TextureFormat::kDepth24Stencil8) {
      flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    } else {
      flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    }
  }

  if (mask & TextureUsage::kShaderRead) {
    flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
  }

  if (mask & TextureUsage::kShaderWrite) {
    flags |= VK_IMAGE_USAGE_STORAGE_BIT;
  }

  return static_cast<VkImageUsageFlagBits>(flags);
}

}  // namespace cubic::vk
