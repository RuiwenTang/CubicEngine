#pragma once

#import <Metal/Metal.h>
#include <cubic/render/bind_group.h>

namespace cubic {

class BindGroupMTL : public BindGroup {
 public:
  BindGroupMTL(id<MTLBuffer> buffer, ShaderStage stage, std::vector<GroupEntry> entries)
      : mBuffer(buffer), mStage(stage), mEntries(std::move(entries)) {}

  ~BindGroupMTL() { [mBuffer release]; }

  id<MTLBuffer> GetNativeBuffer() const { return mBuffer; }

  ShaderStage GetStage() const { return mStage; }

  static std::shared_ptr<BindGroup> Create(const std::shared_ptr<BindGroupLayout>& layout,
                                           std::vector<GroupEntry> entries, id<MTLDevice> device);

 private:
  id<MTLBuffer> mBuffer;
  ShaderStage mStage;
  std::vector<GroupEntry> mEntries;
};

}  // namespace cubic
