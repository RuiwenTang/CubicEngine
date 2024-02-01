#pragma once

#include <cubic/render/texture.h>

// metal header
#import <Metal/Metal.h>

#include <memory>

namespace cubic {

class TextureMTL : public Texture {
 public:
  TextureMTL(const TextureDescirptor& desc, id<MTLTexture> texture);

  ~TextureMTL() override = default;

  id<MTLTexture> GetNativeTexture() const { return mNativeTexture; }

  static std::shared_ptr<Texture> Create(TextureDescirptor* desc, id<MTLDevice> device);

 private:
  id<MTLTexture> mNativeTexture;
};

}  // namespace cubic
