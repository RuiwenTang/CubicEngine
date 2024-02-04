#pragma once

#import <Metal/Metal.h>
#include <cubic/render/buffer.h>

#include <memory>

namespace cubic {

class BufferMTL : public Buffer {
 public:
  BufferMTL(const BufferDescriptor& desc, id<MTLBuffer> buffer) : Buffer(desc), mBuffer(buffer) {}

  ~BufferMTL() override { [mBuffer release]; }

  id<MTLBuffer> GetNativeBuffer() const { return mBuffer; }

  static std::shared_ptr<Buffer> Create(BufferDescriptor* desc, id<MTLDevice> gpu);

 private:
  id<MTLBuffer> mBuffer;
};

}  // namespace cubic
