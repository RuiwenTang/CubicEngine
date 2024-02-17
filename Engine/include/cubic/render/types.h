#pragma once

#include <cstdint>
#include <vector>

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
using BufferUsageMask = uint32_t;
using ShaderStageMask = uint32_t;

enum TextureUsage : TextureUsageMask {
  kTextureCopySrc = 0x1,
  kTextureCopyDst = (0x1 << 1),
  kShaderRead = (0x1 << 2),
  kShaderWrite = (0x1 << 3),
  kRenderTarget = (0x1 << 4),
};

enum BufferUsage : BufferUsageMask {
  kBuffCopySrc = 0x1,
  kBuffCopyDst = (0x1 << 1),
  kBuffUniform = (0x1 << 2),
  kBuffStorage = (0x1 << 3),
  kBuffIndex = (0x1 << 4),
  kBuffVertex = (0x1 << 5),
};

enum ShaderStage : ShaderStageMask {
  kNoneShader = 0,
  kVertexShader = 0x1,
  kFragmentShader = (0x1 << 1),
  kComputeShader = (0x1 << 2),
};

enum class BufferStorageMode {
  kGPUOnly,
  kCPUOnly,
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

enum class VertexFormat {
  kFloat32,
  kFloat32x2,
  kFloat32x3,
  kFloat32x4,
};

enum class VertexStepMode {
  kVertex,
  kInstance,
};

struct VertexAttribute {
  VertexFormat format = VertexFormat::kFloat32;
  uint64_t offset = 0;
  uint32_t location = 0;
};

struct VertexBufferLayout {
  uint64_t stride = 0;
  VertexStepMode stepMode = VertexStepMode::kVertex;
  std::vector<VertexAttribute> attribute = {};
};

}  // namespace cubic
