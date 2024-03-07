#pragma once

#include <cubic/render/buffer.h>

#include <unordered_map>

namespace cubic {

struct BufferRange {
  uint64_t offset = 0;
  uint64_t length = 0;
};

class StageBuffer {
 public:
  StageBuffer(uint64_t minOffset) : mMinOffset(minOffset) {}

  ~StageBuffer() = default;

  BufferRange Allocate(uint64_t size, bool alighOffset = false);

  uint64_t GetCurrentSize() const { return mCurrOffset; }

 private:
  void AlignOffset();

 private:
  uint64_t mMinOffset;
  uint64_t mCurrOffset = 0;
};

}  // namespace cubic
