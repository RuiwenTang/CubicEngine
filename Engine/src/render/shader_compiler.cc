#include "render/shader_compiler.h"

#include <cubic/core/log.h>

namespace cubic {

ShaderCompiler& ShaderCompiler::SetTargetStage(ShaderStage stage) {
  mStage = stage;

  return *this;
}

ShaderCompiler& ShaderCompiler::SetDefinition(const std::string& name, const std::string& value) {
  mOptions.AddMacroDefinition(name, value);

  return *this;
}

std::vector<uint32_t> ShaderCompiler::Compile(const std::string& label, const std::string& source) {
  if (mStage == ShaderStage::kNone) {
    CUB_ERROR("Can not compile shader [ {} ] with None ShaderStage !!", label);

    return {};
  }

  if (source.empty()) {
    CUB_ERROR("Can not compile shader [ {} ] with empty source !!", label);

    return {};
  }

  shaderc::Compiler compiler{};

  auto result = compiler.CompileGlslToSpv(source, GetShadercStage(), label.c_str(), mOptions);

  CUB_DEBUG("Compile shader: [ {} ] with source: {}", label, source);

  if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
    CUB_ERROR("Compile shader: [ {} ] failed. Error: {}", label, result.GetErrorMessage());

    return {};
  }

  return {result.cbegin(), result.cend()};
}

shaderc_shader_kind ShaderCompiler::GetShadercStage() const {
  if (mStage == ShaderStage::kVertex) {
    return shaderc_shader_kind::shaderc_vertex_shader;
  } else if (mStage == ShaderStage::kFragment) {
    return shaderc_shader_kind::shaderc_fragment_shader;
  } else if (mStage == ShaderStage::kCompute) {
    return shaderc_shader_kind::shaderc_compute_shader;
  }

  return shaderc_shader_kind::shaderc_anyhit_shader;
}

}  // namespace cubic
