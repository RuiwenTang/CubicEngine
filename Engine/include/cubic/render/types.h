#pragma once

#include <cstdint>

namespace cubic {

enum class TextureFormat {
  kInvalid,
  kR8Unorm,
  kRGB8Unorm,
  kRGBA8Unorm,
  kBGRA8Unorm,
};

using TextureUsageMask = uint32_t;

enum TextureUsage : TextureUsageMask {
  kCopySrc = 0x1,
  kCopyDst = (0x1 << 1),
  kShaderRead = (0x1 << 2),
  kShaderWrite = (0x1 << 3),
  kRenderTarget = (0x1 << 4),
};

enum class LoadOp {
  kUndefined,
  kClear,
  kLoad,
};

enum class StoreOp {
  kUndefined,
  kStore,
  kDiscard,
};

struct GPUColor {
  float r = 0.0;
  float g = 0.0;
  float b = 0.0;
  float a = 0.0;
};

}  // namespace cubic
