#include "render/mtl/buffer_mtl.h"

#include <cubic/core/log.h>

namespace cubic {

std::shared_ptr<Buffer> BufferMTL::Create(BufferDescriptor *desc, id<MTLDevice> gpu) {
  MTLResourceOptions options = 0;

  if (desc->storageMode == BufferStorageMode::kCPUOnly) {
    options = MTLCPUCacheModeDefaultCache | MTLResourceCPUCacheModeWriteCombined;
  } else {
    options = MTLResourceStorageModePrivate;
  }

  id<MTLBuffer> buffer = nil;

  if (desc->storageMode == BufferStorageMode::kCPUOnly && desc->data) {
    buffer = [gpu newBufferWithBytes:desc->data length:desc->size options:options];
  } else {
    buffer = [gpu newBufferWithLength:desc->size options:options];
  }

  if (buffer == nil) {
    CUB_ERROR("[Metal backend] failed create buffer for size: {}", desc->size);
    return {};
  }

  return std::make_shared<BufferMTL>(*desc, buffer);
}

}
