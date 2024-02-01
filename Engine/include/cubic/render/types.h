#pragma once

#include <cstdint>

namespace cubic {

enum class TextureFormat {
  kInvalid,
  kR8Unorm,
  kRGB8Unorm,
  kRGBA8Unorm,
  kBGRA8Unorm,
  kStencil8,
  kDepth24,
  kDepth24Stencil8,
};

uint32_t BytesPerPixel(TextureFormat format);

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
  kMSAAResolve,
};

struct GPUColor {
  float r = 0.0;
  float g = 0.0;
  float b = 0.0;
  float a = 0.0;
};

enum class BlendOp {
  kAdd,
  kSubtract,
  kReverseSubtract,
  kMin,
  kMax,
};

enum class BlendFactor {
  kZero,
  kOne,
  kSrc,
  kOneMinusSrc,
  kSrcAlpha,
  kOneMinusSrcAlpha,
  kDst,
  kOneMinusDst,
  kDstAlpha,
  kOneMinusDstAlpha,
};

enum class CompareFunction {
  kUndefined,
  kNever,
  kLess,
  kLessEqual,
  kGreater,
  kGreaterEqual,
  kEqual,
  kNotEqual,
  kAlways,
};

enum class FrontFace {
  kCCW,
  kCW,
};

enum class FaceCullMode {
  kNone,
  kFront,
  kBack,
};

enum ColorWriteMask : uint32_t {
  kNone = 0x0,
  kRed = 0x1,
  kGreen = 0x2,
  kBlue = 0x4,
  kAlpha = 0x8,
  kAll = kRed | kGreen | kBlue | kAlpha,
};

enum class StencilOp {
  kKeep,
  kZero,
  kReplace,
  kInvert,
  kIncrementClamp,
  kDecrementClamp,
  kIncrementWrap,
  kDecrementWrap,
};

struct BlendComponent {
  BlendOp operation = BlendOp::kAdd;
  BlendFactor srcFactor = BlendFactor::kSrc;
  BlendFactor dstFactor = BlendFactor::kOneMinusSrc;
};

struct BlendState {
  BlendComponent color = {};
  BlendComponent alpha = {};
};

struct StencilFaceState {
  CompareFunction compare = CompareFunction::kAlways;
  StencilOp failOp = StencilOp::kKeep;
  StencilOp depthFailOp = StencilOp::kKeep;
  StencilOp passOp = StencilOp::kKeep;
};

}  // namespace cubic
