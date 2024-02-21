#include "render/mtl/bind_group_mtl.h"
#include "render/mtl/buffer_mtl.h"
#include "render/mtl/texture_mtl.h"

namespace cubic {

std::shared_ptr<BindGroup> BindGroupMTL::Create(const std::shared_ptr<BindGroupLayout>& layout,
                                                std::vector<GroupEntry> entries, id<MTLDevice> device) {
  if (entries.empty()) {
    return {};
  }

  NSMutableArray* arr_desc = [NSMutableArray new];

  for (const auto& entry : entries) {
    MTLArgumentDescriptor* desc = [MTLArgumentDescriptor argumentDescriptor];

    if (entry.type == EntryType::kUniformBuffer) {
      desc.dataType = MTLDataTypePointer;
    } else if (entry.type == EntryType::kTexture) {
      desc.dataType = MTLDataTypeTexture;
      desc.textureType = MTLTextureType2D;
    } else if (entry.type == EntryType::kSampler) {
      desc.dataType = MTLDataTypeSampler;
    }

    desc.index = entry.binding;
    desc.access = MTLBindingAccessReadOnly;

    [arr_desc addObject:desc];
    [desc release];
  }

  id<MTLArgumentEncoder> encoder = [device newArgumentEncoderWithArguments:arr_desc];

  [arr_desc release];

  if (encoder == nil) {
    return {};
  }

  id<MTLBuffer> buffer = [device newBufferWithLength:encoder.encodedLength options:0];

  if (buffer == nil) {
    return {};
  }

  [encoder setArgumentBuffer:buffer offset:0];

  for (const auto& entry : entries) {
    if (entry.type == EntryType::kUniformBuffer) {
      auto const& buffer_view = std::get<BufferView>(entry.resource);

      [encoder setBuffer:dynamic_cast<BufferMTL*>(buffer_view.buffer.get())->GetNativeBuffer()
                  offset:buffer_view.offset
                 atIndex:entry.binding];
    } else if (entry.type == EntryType::kTexture) {
      auto const& texture = std::get<std::shared_ptr<Texture>>(entry.resource);

      [encoder setTexture:dynamic_cast<TextureMTL*>(texture.get())->GetNativeTexture() atIndex:entry.binding];
    }
  }

  // TODO: support group cross shader stages
  ShaderStage stage = ShaderStage::kNoneShader;

  if (layout->GetEntries().front().visibility & ShaderStage::kVertexShader) {
    stage = ShaderStage::kVertexShader;
  } else {
    stage = ShaderStage::kFragmentShader;
  }

  return std::make_shared<BindGroupMTL>(buffer, stage, std::move(entries));
}

}  // namespace cubic
