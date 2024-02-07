#pragma once

#include <cubic/render/buffer.h>
#include <vk_mem_alloc.h>
#include <volk.h>

#include <memory>

namespace cubic {

class VulkanDevice;

class BufferVK : public Buffer {
 public:
  BufferVK(const BufferDescriptor& desc, VkBuffer buffer, VmaAllocation allocation, VmaAllocator allocator);
  ~BufferVK() override;

  VkBuffer GetNativeBuffer() const { return mBuffer; }

  static std::shared_ptr<Buffer> Create(BufferDescriptor* desc, VmaAllocator allocator);

 private:
  VkBuffer mBuffer;
  VmaAllocation mAllocation;
  VmaAllocator mAllocator;
};

}  // namespace cubic
