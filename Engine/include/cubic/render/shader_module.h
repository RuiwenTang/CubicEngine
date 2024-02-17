#pragma once

#include <cubic/platform.h>
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
  ShaderModule(ShaderStage stage, std::string label) : mStage(stage), mLabel(std::move(label)) {}

  virtual ~ShaderModule() = default;

  ShaderStage GetStage() const { return mStage; }

  const std::string& GetLabel() const { return mLabel; }

 private:
  ShaderStage mStage;
  std::string mLabel;
};

}  // namespace cubic
