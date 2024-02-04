#pragma once

#include <cubic/platform.h>
#include <cubic/render/types.h>

namespace cubic {

struct BufferDescriptor {
  BufferUsageMask usage = BufferUsage::kBuffCopySrc;
  BufferStorageMode storageMode = BufferStorageMode::kCPUOnly;
  uint64_t size = 0;
};

class CUBIC_API Buffer {
 public:
  Buffer(const BufferDescriptor& desc) : mDesc(desc) {}

  virtual ~Buffer() = default;

  const BufferDescriptor& GetDescriptor() const { return mDesc; }

 private:
  BufferDescriptor mDesc;
};

}  // namespace cubic
