#pragma once

#include <cubic/platform.h>
#include <cubic/render/types.h>

#include <memory>

namespace cubic {

struct BufferDescriptor {
  BufferUsageMask usage = BufferUsage::kBuffCopySrc;
  BufferStorageMode storageMode = BufferStorageMode::kCPUOnly;
  uint64_t size = 0;
  void* data = nullptr;
};

class CUBIC_API Buffer {
 public:
  Buffer(const BufferDescriptor& desc) : mDesc(desc) {}

  virtual ~Buffer() = default;

  const BufferDescriptor& GetDescriptor() const { return mDesc; }

 private:
  BufferDescriptor mDesc;
};

struct BufferView {
  std::shared_ptr<Buffer> buffer = {};
  uint64_t offset = 0;
  uint64_t length = 0;
};

}  // namespace cubic
