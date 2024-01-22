#pragma once

#include <cubic/render/shader_module.h>

#include <shaderc/shaderc.hpp>
#include <vector>

namespace cubic {

class ShaderCompiler {
 public:
  ShaderCompiler() = default;
  ~ShaderCompiler() = default;

  ShaderCompiler& SetTargetStage(ShaderStage stage);

  ShaderCompiler& SetDefinition(const std::string& name, const std::string& value);

  std::vector<uint32_t> Compile(const std::string& label, const std::string& source);

 private:
  shaderc_shader_kind GetShadercStage() const;

 private:
  ShaderStage mStage = ShaderStage::kNone;
  shaderc::CompileOptions mOptions = {};
};

}  // namespace cubic
