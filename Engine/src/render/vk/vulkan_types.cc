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

}  // namespace cubic::vk
