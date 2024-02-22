#pragma once

#import <Metal/Metal.h>
#include <cubic/render/bind_group.h>

namespace cubic {

struct StageBindingMTL {
  ShaderStage stage = ShaderStage::kNoneShader;
  uint64_t offset = 0;
};

class BindGroupMTL : public BindGroup {
 public:
  BindGroupMTL(id<MTLBuffer> buffer, std::vector<GroupEntry> entries, std::vector<StageBindingMTL> stageBinding)
      : mBuffer(buffer), mEntries(std::move(entries)), mStageBinding(std::move(stageBinding)) {}

  ~BindGroupMTL() { [mBuffer release]; }

  void Bind(id<MTLRenderCommandEncoder> encoder, uint32_t slot);

  static std::shared_ptr<BindGroup> Create(const std::shared_ptr<BindGroupLayout>& layout,
                                           std::vector<GroupEntry> entries, id<MTLDevice> device);

 private:
  id<MTLBuffer> mBuffer;
  std::vector<GroupEntry> mEntries;
  std::vector<StageBindingMTL> mStageBinding;
};

struct ArgumentBufferEncoder {
  id<MTLArgumentEncoder> encoder = nil;

  ArgumentBufferEncoder(id<MTLArgumentEncoder> e) : encoder(e) {}

  ~ArgumentBufferEncoder() { [encoder release]; }

  static std::unique_ptr<ArgumentBufferEncoder> Init(id<MTLDevice> device, const std::vector<GroupEntry>& entries);

  void Encode(id<MTLBuffer> buffer, uint64_t offset, const std::vector<GroupEntry>& entries);
};

}  // namespace cubic
