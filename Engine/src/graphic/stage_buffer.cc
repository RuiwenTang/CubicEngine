#include "graphic/stage_buffer.h"

namespace cubic {

BufferRange StageBuffer::Allocate(uint64_t size, bool alighOffset) {
  if (alighOffset) {
    AlignOffset();
  }

  BufferRange range{};
  range.offset = mCurrOffset;
  range.length = size;

  mCurrOffset += size;

  return range;
}

void StageBuffer::AlignOffset() {
  if (mCurrOffset <= mMinOffset) {
    mCurrOffset = mMinOffset;

    return;
  }

  uint64_t deita = mCurrOffset % mMinOffset;

  mCurrOffset += (mMinOffset - deita);
}

}  // namespace cubic
