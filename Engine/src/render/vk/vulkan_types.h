#pragma once

#include <cubic/render/types.h>
#include <volk.h>

namespace cubic {

namespace vk {

TextureFormat TypeConvert(VkFormat format);

VkFormat TypeConvert(TextureFormat format);

VkAttachmentLoadOp TypeConvert(LoadOp op);

VkAttachmentStoreOp TypeConvert(StoreOp op);

VkSampleCountFlagBits TypeConvert(uint32_t count);

VkClearValue TypeConvert(const GPUColor& color);

VkClearValue TypeConvert(float depth, uint32_t stencil);

}  // namespace vk

}  // namespace cubic
