#include <cubic/graphic/material/color_material.h>
#include <fmt/core.h>

namespace cubic {

ColorMaterial::ColorMaterial(float red, float green, float blue, float alpha) : mColor({red, green, blue, alpha}) {}

std::vector<MaterialInput> ColorMaterial::GetInput() const {
  return {MaterialInput{
              "position",
              VertexFormat::kFloat32x3,
          },
          MaterialInput{
              "normal",
              VertexFormat::kFloat32x3,
          }};
}

std::vector<GroupEntryInfo> ColorMaterial::GetResourceInfo() const {
  return {GroupEntryInfo{
      0,
      EntryType::kUniformBuffer,
      ShaderStage::kFragmentShader,
  }};
}

std::vector<ResourceData> ColorMaterial::GetResourceData() const {
  return {ResourceData{
      const_cast<float*>(mColor.data()),
      mColor.size() * sizeof(float),
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

std::string ColorMaterial::GenColorFunction() const {
  return R"(
    vec3 eyeDir = normalize(vec3(0.0, 0.0, 1.0) - position);
    vec3 norm = normalize(normal);
    float diff = max(dot(norm, eyeDir), 0.0);

    vec3 diffuse = vec3(1.0, 1.0, 1.0) * diff;

    return vec4(uCustomColor.color.rgb * diffuse, uCustomColor.color.a);
    )";
}

}  // namespace cubic
