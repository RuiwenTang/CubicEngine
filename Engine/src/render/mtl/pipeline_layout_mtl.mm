#include "render/mtl/pipeline_layout_mtl.h"
#include "render/mtl/buffer_mtl.h"

namespace cubic {

namespace {

void SetEntryToEncoder(id<MTLRenderCommandEncoder> encoder, const GroupEntry& entry, uint32_t slot, ShaderStage stage) {
  switch (entry.type) {
    case EntryType::kUniformBuffer:
    case EntryType::kStorgeBuffer: {
      auto buffer_view = std::get<BufferView>(entry.resource);
      auto buffer = std::dynamic_pointer_cast<BufferMTL>(buffer_view.buffer);
      if (buffer == nullptr) {
        return;
      }

      if (stage & ShaderStage::kVertexShader) {
        [encoder setVertexBuffer:buffer->GetNativeBuffer() offset:buffer_view.offset atIndex:slot];
      }

      if (stage & ShaderStage::kFragmentShader) {
        [encoder setFragmentBuffer:buffer->GetNativeBuffer() offset:buffer_view.offset atIndex:slot];
      }
    } break;
    case EntryType::kSampler:
      break;
    case EntryType::kTexture:
      break;
    case EntryType::kCombinedImageSampler:
      break;
    default:
      break;
  }
}

}  // namespace

void PipelineLayoutMTL::SetBindGroup(id<MTLRenderCommandEncoder> encoder, uint32_t slot, const BindGroup& group) const {
  // check if vertex stage has group
  {
    auto it = std::find_if(mVSGroups.begin(), mVSGroups.end(), [slot](const StageGroup& g) { return g.set == slot; });
    if (it != mVSGroups.end()) {
      const auto& sg = *it;

      for (const auto& entry : sg.entries) {
        auto ug = group.GetEntry(entry.binding);

        if (ug != nullptr) {
          SetEntryToEncoder(encoder, *ug, entry.slot, ShaderStage::kVertexShader);
        }
      }
    }
  }

  // check if fragment stage has group
  {
    auto it = std::find_if(mFSGroups.begin(), mFSGroups.end(), [slot](const StageGroup& g) { return g.set == slot; });
    if (it != mFSGroups.end()) {
      const auto& sg = *it;

      for (const auto& entry : sg.entries) {
        auto ug = group.GetEntry(entry.binding);

        if (ug != nullptr) {
          SetEntryToEncoder(encoder, *ug, entry.slot, ShaderStage::kFragmentShader);
        }
      }
    }
  }
}

}  // namespace cubic
