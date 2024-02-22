#include "render/mtl/bind_group_mtl.h"

#include <functional>

#include "render/mtl/buffer_mtl.h"
#include "render/mtl/texture_mtl.h"

namespace cubic {

void BindGroupMTL::Bind(id<MTLRenderCommandEncoder> encoder, uint32_t slot) {
  for (const auto& binding : mStageBinding) {
    if (binding.stage & ShaderStage::kVertexShader) {
      [encoder setVertexBuffer:mBuffer offset:binding.offset atIndex:slot];
    } else if (binding.stage & ShaderStage::kFragmentShader) {
      [encoder setFragmentBuffer:mBuffer offset:binding.offset atIndex:slot];
    }
  }
}

std::shared_ptr<BindGroup> BindGroupMTL::Create(const std::shared_ptr<BindGroupLayout>& layout,
                                                std::vector<GroupEntry> entries, id<MTLDevice> device) {
  if (entries.empty()) {
    return {};
  }

  std::vector<GroupEntry> vs_entries{};
  std::vector<GroupEntry> fs_entries{};

  auto get_entry_by_binding = [&entries](uint32_t binding) -> GroupEntry {
    for (const auto& entry : entries) {
      if (entry.binding == binding) {
        return entry;
      }
    }

    return {};
  };

  for (auto const& entry : layout->GetEntries()) {
    if (entry.visibility & ShaderStage::kVertexShader) {
      vs_entries.emplace_back(get_entry_by_binding(entry.binding));
    }

    if (entry.visibility & ShaderStage::kFragmentShader) {
      fs_entries.emplace_back(get_entry_by_binding(entry.binding));
    }
  }

  auto vs_encoder = ArgumentBufferEncoder::Init(device, vs_entries);
  auto fs_encoder = ArgumentBufferEncoder::Init(device, fs_entries);

  uint64_t length = 0;

  if (vs_encoder) {
    length += vs_encoder->encoder.encodedLength;
  }

  if (fs_encoder) {
    length += fs_encoder->encoder.encodedLength;
  }

  id<MTLBuffer> buffer = [device newBufferWithLength:length options:0];

  if (buffer == nil) {
    return {};
  }

  std::vector<StageBindingMTL> bindings{};

  if (vs_encoder) {
    bindings.emplace_back(StageBindingMTL{ShaderStage::kVertexShader, 0});
    vs_encoder->Encode(buffer, 0, vs_entries);
  }

  if (fs_encoder) {
    uint64_t offset = vs_encoder ? vs_encoder->encoder.encodedLength : 0;

    bindings.emplace_back(StageBindingMTL{ShaderStage::kFragmentShader, offset});

    fs_encoder->Encode(buffer, offset, fs_entries);
  }

  return std::make_shared<BindGroupMTL>(buffer, std::move(entries), std::move(bindings));
}

std::unique_ptr<ArgumentBufferEncoder> ArgumentBufferEncoder::Init(id<MTLDevice> device,
                                                                   const std::vector<GroupEntry>& entries) {
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

  return std::make_unique<ArgumentBufferEncoder>(encoder);
}

void ArgumentBufferEncoder::Encode(id<MTLBuffer> buffer, uint64_t offset, const std::vector<GroupEntry>& entries) {
  [encoder setArgumentBuffer:buffer offset:offset];

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
}

}  // namespace cubic
