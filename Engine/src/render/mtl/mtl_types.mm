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

MTLPixelFormat TypeConvert(TextureFormat format) {
  switch (format) {
    case TextureFormat::kInvalid:
      return MTLPixelFormatInvalid;
    case TextureFormat::kR8Unorm:
      return MTLPixelFormatR8Unorm;
    case TextureFormat::kRGB8Unorm:
      return MTLPixelFormatRGBA8Unorm_sRGB;
    case TextureFormat::kRGBA8Unorm:
      return MTLPixelFormatRGBA8Unorm;
    case TextureFormat::kBGRA8Unorm:
      return MTLPixelFormatBGRA8Unorm;
    case TextureFormat::kStencil8:
      return MTLPixelFormatStencil8;
    case TextureFormat::kDepth24:
    case TextureFormat::kDepth24Stencil8:
      return MTLPixelFormatDepth24Unorm_Stencil8;
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

MTLBlendOperation TypeConvert(BlendOp op) {
  switch (op) {
    case BlendOp::kAdd:
      return MTLBlendOperationAdd;
    case BlendOp::kSubtract:
      return MTLBlendOperationSubtract;
    case BlendOp::kReverseSubtract:
      return MTLBlendOperationReverseSubtract;
    case BlendOp::kMin:
      return MTLBlendOperationMin;
    case BlendOp::kMax:
      return MTLBlendOperationMax;
  }
}

MTLBlendFactor TypeConvert(BlendFactor factor) {
  switch (factor) {
    case BlendFactor::kZero:
      return MTLBlendFactorZero;
    case BlendFactor::kOne:
      return MTLBlendFactorOne;
    case BlendFactor::kSrc:
      return MTLBlendFactorSourceColor;
    case BlendFactor::kOneMinusSrc:
      return MTLBlendFactorOneMinusSourceColor;
    case BlendFactor::kSrcAlpha:
      return MTLBlendFactorSourceAlpha;
    case BlendFactor::kOneMinusSrcAlpha:
      return MTLBlendFactorOneMinusSourceAlpha;
    case BlendFactor::kDst:
      return MTLBlendFactorDestinationColor;
    case BlendFactor::kOneMinusDst:
      return MTLBlendFactorOneMinusDestinationColor;
    case BlendFactor::kDstAlpha:
      return MTLBlendFactorDestinationAlpha;
    case BlendFactor::kOneMinusDstAlpha:
      return MTLBlendFactorOneMinusDestinationAlpha;
  }
}

MTLTextureUsage TypeConvert(TextureUsageMask usage) {
  MTLTextureUsage mtl_usage = 0;

  if (usage & TextureUsage::kShaderRead) {
    mtl_usage |= MTLTextureUsageShaderRead;
  }

  if (usage & TextureUsage::kShaderWrite) {
    mtl_usage |= MTLTextureUsageShaderWrite;
  }

  if (usage & TextureUsage::kRenderTarget) {
    mtl_usage |= MTLTextureUsageRenderTarget;
  }

  return mtl_usage;
}

MTLVertexFormat TypeConvert(VertexFormat format) {
  switch (format) {
    case VertexFormat::kFloat32:
      return MTLVertexFormatFloat;
    case VertexFormat::kFloat32x2:
      return MTLVertexFormatFloat2;
    case VertexFormat::kFloat32x3:
      return MTLVertexFormatFloat3;
    case VertexFormat::kFloat32x4:
      return MTLVertexFormatFloat4;
  }
}

MTLVertexStepFunction TypeConvert(VertexStepMode stepMode) {
  switch (stepMode) {
    case VertexStepMode::kVertex:
      return MTLVertexStepFunctionPerVertex;
    case VertexStepMode::kInstance:
      return MTLVertexStepFunctionPerInstance;
  }
}

MTLCompareFunction TypeConvert(CompareFunction func) {
  switch (func) {
    case CompareFunction::kNever:
      return MTLCompareFunctionNever;
    case CompareFunction::kLess:
      return MTLCompareFunctionLess;
    case CompareFunction::kLessEqual:
      return MTLCompareFunctionLessEqual;
    case CompareFunction::kGreater:
      return MTLCompareFunctionGreater;
    case CompareFunction::kGreaterEqual:
      return MTLCompareFunctionGreaterEqual;
    case CompareFunction::kEqual:
      return MTLCompareFunctionEqual;
    case CompareFunction::kNotEqual:
      return MTLCompareFunctionNotEqual;
    case CompareFunction::kAlways:
      return MTLCompareFunctionAlways;
    default:
      return MTLCompareFunctionNever;
  }
}

}  // namespace cubic
