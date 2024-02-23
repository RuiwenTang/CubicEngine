#include <cubic/graphic/material/color_material.h>
#include <fmt/core.h>

namespace cubic {

ColorMaterial::ColorMaterial(float red, float green, float blue, float alpha) : mColor({red, green, blue, alpha}) {}

std::vector<MaterialInput> ColorMaterial::GetInput() const { return {}; }

std::vector<GroupEntryInfo> ColorMaterial::GetResourceInfo() const {
  return {GroupEntryInfo{
      0,
      EntryType::kUniformBuffer,
      ShaderStage::kFragmentShader,
  }};
}

std::string ColorMaterial::GenResourceSet(uint32_t index) const {
  return fmt::format(R"(
                layout(set = {}, binding = 0) uniform CustomColor {{
                   vec4 color;
                }} uCustomColor;
        )",
                     index);
}

std::string ColorMaterial::GenColorFunction(const std::string& name) const {
  return fmt::format(R"(
                vec4 {} () {{
                  return uCustomColor.color;
                }}
                )",
                     name);
}

}  // namespace cubic
