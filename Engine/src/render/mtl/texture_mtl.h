#pragma once

#include <cubic/render/texture.h>

// metal header
#import <Metal/Metal.h>

namespace cubic {

class TextureMTL : public Texture {
 public:
  TextureMTL(const TextureDescirptor& desc, id<MTLTexture> texture, id<MTLDevice> device, id<MTLCommandQueue> queue);

  ~TextureMTL() override = default;

  void UploadData(void* data, uint32_t w, uint32_t h, uint32_t x, uint32_t y) override;

  id<MTLTexture> GetNativeTexture() const { return mNativeTexture; }

 private:
  id<MTLTexture> mNativeTexture;
  id<MTLDevice> mDevice;
  id<MTLCommandQueue> mQueue;
};

}  // namespace cubic
