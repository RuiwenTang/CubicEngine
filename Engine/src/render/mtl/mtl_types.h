#import <Metal/Metal.h>
#include <cubic/render/types.h>

namespace cubic {

enum {
  kVertexIndexOffset = 16,
};

TextureFormat TypeConvert(MTLPixelFormat format);

MTLPixelFormat TypeConvert(TextureFormat format);

MTLClearColor TypeConvert(const GPUColor& color);

MTLLoadAction TypeConvert(LoadOp op);

MTLStoreAction TypeConvert(StoreOp op);

MTLBlendOperation TypeConvert(BlendOp op);

MTLBlendFactor TypeConvert(BlendFactor factor);

MTLTextureUsage TypeConvert(TextureUsageMask usage);

MTLVertexFormat TypeConvert(VertexFormat format);

MTLVertexStepFunction TypeConvert(VertexStepMode stepMode);

MTLCompareFunction TypeConvert(CompareFunction func);

}  // namespace cubic
