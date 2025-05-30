#pragma once

#include <cubic/render/types.h>
#include <cubic/render/bind_group.h>
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

VkColorComponentFlags TypeConvert(ColorWriteMask mask);

VkBlendFactor TypeConvert(BlendFactor factor);

VkBlendOp TypeConvert(BlendOp op);

VkImageUsageFlagBits TypeConvert(TextureUsageMask mask, TextureFormat format);

VkBufferUsageFlags TypeConvertForBuffer(BufferUsageMask usage);

VkVertexInputRate TypeConvert(VertexStepMode mode);

VkFormat TypeConvertForInput(VertexFormat format);

VkDescriptorType TypeConvert(EntryType type);

VkShaderStageFlags TypeConvertForBinding(ShaderStageMask mask);

VkCompareOp TypeConvert(CompareFunction op);

VkFilter TypeConvert(FilterType filter);

VkSamplerAddressMode TypeConvert(AddressMode mode);

}  // namespace vk

}  // namespace cubic
