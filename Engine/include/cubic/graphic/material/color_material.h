#pragma once

#include <cubic/graphic/material.h>

#include <array>

namespace cubic {

class CUBIC_API ColorMaterial : public Material {
 public:
  ColorMaterial(float red, float green, float blue, float alpha);

  virtual ~ColorMaterial() = default;

  std::vector<MaterialInput> GetInput() const override;

  std::vector<GroupEntryInfo> GetResourceInfo() const override;

  std::vector<ResourceData> GetResourceData() const override;

  std::string GenResourceSet(uint32_t index) const override;

  std::string GenColorFunction() const override;

 private:
  std::array<float, 4> mColor = {};
};

}  // namespace cubic
