#include "render/mtl/texture_mtl.h"

#include <Metal/Metal.h>
#include <cubic/core/log.h>

namespace cubic {

TextureMTL::TextureMTL(const TextureDescirptor& desc, id<MTLTexture> texture, id<MTLDevice> device,
                       id<MTLCommandQueue> queue)
    : Texture(desc), mNativeTexture(texture), mDevice(device), mQueue(queue) {}

void TextureMTL::UploadData(void* data, uint32_t w, uint32_t h, uint32_t x, uint32_t y) {
  const auto& desc = GetDescriptor();

  if ((desc.usage & TextureUsage::kShaderRead) == 0 || (desc.usage & TextureUsage::kCopySrc)) {
    CUB_ERROR("Only Texture usage for ShaderRead or CopySrc support update data by user");
    return;
  }

  if (mQueue == nil) {
    CUB_ERROR("Metal Command Queue is nil !!");
    return;
  }

  auto bpp = [mNativeTexture bufferBytesPerRow];

  id<MTLBuffer> stage_buffer = [mDevice newBufferWithBytes:data
                                                    length:w * h * bpp
                                                   options:MTLResourceStorageModeShared];

  id<MTLCommandBuffer> cmd = [mQueue commandBuffer];

  id<MTLBlitCommandEncoder> encoder = [cmd blitCommandEncoder];

  [encoder copyFromBuffer:stage_buffer
             sourceOffset:0
        sourceBytesPerRow:bpp
      sourceBytesPerImage:bpp
               sourceSize:MTLSizeMake(w, h, 1)
                toTexture:mNativeTexture
         destinationSlice:bpp
         destinationLevel:0
        destinationOrigin:MTLOriginMake(x, y, 0)];

  [encoder endEncoding];

  [cmd commit];
}

}
