#import <Metal/Metal.h>
#include <cubic/render/types.h>

namespace cubic {

TextureFormat TypeConvert(MTLPixelFormat format);

MTLClearColor TypeConvert(const GPUColor& color);

MTLLoadAction TypeConvert(LoadOp op);

MTLStoreAction TypeConvert(StoreOp op);

}  // namespace cubic
