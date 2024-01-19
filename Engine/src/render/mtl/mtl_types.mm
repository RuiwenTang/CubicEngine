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

MTLClearColor TypeConvert(const GPUColor& color) { return MTLClearColorMake(color.r, color.g, color.b, color.a); }

MTLLoadAction TypeConvert(LoadOp op) {
  switch (op) {
    case LoadOp::kLoad:
      return MTLLoadActionLoad;
    case LoadOp::kClear:
      return MTLLoadActionClear;
    case LoadOp::kUndefined:
      return MTLLoadActionDontCare;
  }
}

MTLStoreAction TypeConvert(StoreOp op) {
  switch (op) {
    case StoreOp::kStore:
      return MTLStoreActionStore;
    case StoreOp::kDiscard:
      return MTLStoreActionDontCare;
    case StoreOp::kUndefined:
      return MTLStoreActionDontCare;
    case StoreOp::kMSAAResolve:
      return MTLStoreActionMultisampleResolve;
  }
}

}
