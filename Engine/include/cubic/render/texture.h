#pragma once

#include <cubic/platform.h>
#include <cubic/render/types.h>

#include <cstddef>

namespace cubic {

struct TextureDescriptor {
  TextureFormat format = TextureFormat::kRGBA8Unorm;
  TextureUsageMask usage = TextureUsage::kShaderRead;
  uint32_t sample_count = 1;
  uint32_t width = 0;
  uint32_t height = 0;
};

class CUBIC_API Texture {
 public:
  Texture(const TextureDescriptor& desc) : mDesc(desc) {}

  virtual ~Texture() = default;

  const TextureDescriptor& GetDescriptor() const { return mDesc; }

 private:
  TextureDescriptor mDesc;
};

}  // namespace cubic
