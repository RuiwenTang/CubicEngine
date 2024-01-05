#pragma once

#include <cubic/platform.h>
#include <cubic/render/types.h>

#include <cstddef>

namespace cubic {

struct TextureDescirptor {
  TextureFormat format = TextureFormat::kRGBA8Unorm;
  TextureUsageMask usage = TextureUsage::kShaderRead;
  uint32_t width = 0;
  uint32_t height = 0;
};

class CUBIC_API Texture {
 public:
  Texture(const TextureDescirptor& desc) : mDesc(desc) {}

  virtual ~Texture() = default;

  virtual void UploadData(void* data, uint32_t w, uint32_t h, uint32_t x, uint32_t y) = 0;

  const TextureDescirptor& GetDescriptor() const { return mDesc; }

 private:
  TextureDescirptor mDesc;
};

}  // namespace cubic
