#include "render/mtl/texture_mtl.h"

#include <Metal/Metal.h>
#include <cubic/core/log.h>
#include <cubic/platform.h>
#include "render/mtl/mtl_types.h"

namespace cubic {

static bool support_memoryless(id<MTLDevice> device) {
  // Refer to the "Memoryless render targets" feature in the table below:
  // https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf
  if (@available(ios 13.0, tvos 13.0, macos 10.15, *)) {
    return [device supportsFamily:MTLGPUFamilyApple2];
  } else {
#if CUBIC_PLATFORM_IOS
    // This is perhaps redundant. But, just in case we somehow get into a case
    // where Impeller runs on iOS versions less than 8.0 and/or without A8
    // GPUs, we explicitly check feature set support.
    return [device supportsFeatureSet:MTLFeatureSet_iOS_GPUFamily2_v1];
#else
    // MacOS devices with Apple GPUs are only available with macos 10.15 and
    // above. So, if we are here, it is safe to assume that memory-less targets
    // are not supported.
    return false;
#endif
  }
}

TextureMTL::TextureMTL(const TextureDescirptor& desc, id<MTLTexture> texture)
    : Texture(desc), mNativeTexture(texture) {}

std::shared_ptr<Texture> TextureMTL::Create(TextureDescirptor* desc, id<MTLDevice> device) {
  MTLTextureDescriptor* mtl_desc = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:TypeConvert(desc->format)
                                                                                      width:desc->width
                                                                                     height:desc->height
                                                                                  mipmapped:NO];

  mtl_desc.sampleCount = desc->sample_count;

  if (desc->sample_count == 1) {
    mtl_desc.storageMode = MTLStorageModePrivate;
  } else {
    mtl_desc.textureType = MTLTextureType2DMultisample;
    mtl_desc.allowGPUOptimizedContents = YES;
    if (support_memoryless(device)) {
      mtl_desc.storageMode = MTLStorageModeMemoryless;
    } else {
      mtl_desc.storageMode = MTLStorageModePrivate;
    }
  }

  mtl_desc.usage = TypeConvert(desc->usage);

  id<MTLTexture> texture = [device newTextureWithDescriptor:mtl_desc];

  [mtl_desc release];

  if (texture == nil) {
    CUB_ERROR("[Metal backend] failed create native texture !!");

    return {};
  }

  return std::make_shared<TextureMTL>(*desc, texture);
}

}  // namespace cubic
