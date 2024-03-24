#include <cubic/render/types.h>

namespace cubic {

uint32_t BytesPerPixel(TextureFormat format) {
  switch (format) {
    case TextureFormat::kInvalid:
      return 0;
    case TextureFormat::kR8Unorm:
    case TextureFormat::kStencil8:
      return 1;
    case TextureFormat::kRGB8Unorm:
    case TextureFormat::kDepth24:
      return 3;
    case TextureFormat::kBGRA8Unorm:
    case TextureFormat::kRGBA8Unorm:
    case TextureFormat::kDepth24Stencil8:
      return 4;
  }
  return 0;
}

}  // namespace cubic
