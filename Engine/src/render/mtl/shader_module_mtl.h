#pragma once

#import <Metal/Metal.h>
#include <cubic/render/shader_module.h>

namespace cubic {

struct StageEntryInfo {
  EntryType type = EntryType::kNone;

  // binding index in shader
  uint32_t binding = 0;

  uint32_t slot = 0;
};

struct StageGroup {
  uint32_t set = 0;

  std::vector<StageEntryInfo> entries;

  bool AddEntry(StageEntryInfo entry);

  BindGroupLayout GenLayout(ShaderStage stage);
};

class ShaderModuleMTL : public ShaderModule {
 public:
  ShaderModuleMTL(ShaderStage stage, std::string label, std::vector<BindGroupLayout> groups,
                  std::vector<StageGroup> stageGroups, id<MTLLibrary> shader);

  ~ShaderModuleMTL() override;

  id<MTLLibrary> GetNativeShader() const;

  id<MTLFunction> GetEntryPoint() const;

  static std::shared_ptr<ShaderModule> Compile(id<MTLDevice> gpu, ShaderModuleDescriptor* desc,
                                               const std::vector<uint32_t>& spv);

  const std::vector<StageGroup>& GetStageGroups() const { return mGroups; }

 private:
  std::vector<StageGroup> mGroups;
  id<MTLLibrary> mNativeShader;
  id<MTLFunction> mEntryPoint = nil;
};

}  // namespace cubic
