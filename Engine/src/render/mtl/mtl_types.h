#import <Metal/Metal.h>
#include <cubic/render/types.h>

namespace cubic {

TextureFormat TypeConvert(MTLPixelFormat format);

MTLPixelFormat TypeConvert(TextureFormat format);

MTLClearColor TypeConvert(const GPUColor& color);

MTLLoadAction TypeConvert(LoadOp op);

MTLStoreAction TypeConvert(StoreOp op);

MTLBlendOperation TypeConvert(BlendOp op);

MTLBlendFactor TypeConvert(BlendFactor factor);

}  // namespace cubic
