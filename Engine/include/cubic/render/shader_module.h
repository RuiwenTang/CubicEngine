#pragma once

#include <cubic/platform.h>
#include <cubic/render/pipeline_layout.h>
#include <cubic/render/types.h>

#include <string>

namespace cubic {

struct ShaderModuleDescriptor {
  ShaderStage stage = ShaderStage::kNoneShader;
  std::string code = {};
  std::string label = {};
};

class CUBIC_API ShaderModule {
 public:
  ShaderModule(ShaderStage stage, std::string label, std::vector<BindGroupLayout> groups)
      : mStage(stage), mLabel(std::move(label)), mGroups(std::move(groups)) {}

  virtual ~ShaderModule() = default;

  ShaderStage GetStage() const { return mStage; }

  const std::string& GetLabel() const { return mLabel; }

  const std::vector<BindGroupLayout>& GetGroups() const { return mGroups; }

 private:
  ShaderStage mStage;
  std::string mLabel;
  std::vector<BindGroupLayout> mGroups;
};

}  // namespace cubic
