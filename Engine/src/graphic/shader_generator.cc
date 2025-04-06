#include "graphic/shader_generator.h"

#include <cubic/render/render_system.h>

#include <algorithm>
#include <cstring>

namespace cubic {

bool ShaderGenerator::Prepare() {
  if (mGeometry == nullptr || mMaterial == nullptr) {
    return false;
  }

  return true;
}

void ShaderGenerator::BuildVertexProgram() {}

void ShaderGenerator::BuildFragmentProgram() {}

std::string ShaderGenerator::GenVertexShader() { return ""; }

std::string ShaderGenerator::GenFragmentShader() {
  std::string source = {};

  return source;
}

uint64_t get_attr_offset(const std::vector<BufferAttribute>& attrs, const uint32_t location) {
  for (uint32_t i = 0; i < attrs.size(); i++) {
    if (attrs[i].attribute.location == location) {
      return attrs[i].attribute.offset;
    }
  }

  return 0;
}

VertexFormat get_attr_format(const std::vector<BufferAttribute>& attrs, const uint32_t location) {
  for (uint32_t i = 0; i < attrs.size(); i++) {
    if (attrs[i].attribute.location == location) {
      return attrs[i].attribute.format;
    }
  }

  return VertexFormat::kFloat32;
}

}  // namespace cubic
