#include "render/mtl/mtl_types.h"

namespace cubic {

TextureFormat TypeConvert(MTLPixelFormat format) {
  switch (format) {
    case MTLPixelFormatBGRA8Unorm:
      return TextureFormat::kBGRA8Unorm;
    case MTLPixelFormatRGBA8Unorm:
      return TextureFormat::kRGBA8Unorm;
    case MTLPixelFormatRGBA8Unorm_sRGB:
      return TextureFormat::kRGB8Unorm;
    case MTLPixelFormatR8Unorm:
      return TextureFormat::kR8Unorm;
    default:
      return TextureFormat::kInvalid;
  }
}

}
