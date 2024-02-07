#include "render/vk/buffer_vk.h"

#include <cubic/core/log.h>

#include <cstring>

#include "render/vk/vulkan_types.h"

namespace cubic {

BufferVK::BufferVK(const BufferDescriptor& desc, VkBuffer buffer, VmaAllocation allocation, VmaAllocator allocator)
    : Buffer(desc), mBuffer(buffer), mAllocation(allocation), mAllocator(allocator) {}

BufferVK::~BufferVK() { vmaDestroyBuffer(mAllocator, mBuffer, mAllocation); }

std::shared_ptr<Buffer> BufferVK::Create(BufferDescriptor* desc, VmaAllocator allocator) {
  VkBufferCreateInfo buffer_info{};
  buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  buffer_info.usage = vk::TypeConvertForBuffer(desc->usage);
  buffer_info.size = desc->size;

  VmaAllocationCreateInfo alloc_info{};

  if (desc->storageMode == BufferStorageMode::kCPUOnly) {
    alloc_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;
  } else {
    alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    alloc_info.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  }

  VkBuffer buffer = VK_NULL_HANDLE;
  VmaAllocation allocation = VK_NULL_HANDLE;

  if (vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &buffer, &allocation, nullptr) != VK_SUCCESS) {
    CUB_ERROR("[Vulkan backend] failed create buffer for size: {} and usage {:x}", desc->size, desc->usage);
    return {};
  }

  if (desc->storageMode == BufferStorageMode::kCPUOnly && desc->data) {
    void* vma_ptr = nullptr;
    vmaMapMemory(allocator, allocation, &vma_ptr);

    std::memcpy(vma_ptr, desc->data, desc->size);

    vmaUnmapMemory(allocator, allocation);
  }

  return std::make_shared<BufferVK>(*desc, buffer, allocation, allocator);
}

}  // namespace cubic
