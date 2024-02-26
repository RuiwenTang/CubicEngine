#include "slang/attribute.h"

#include <fmt/core.h>

namespace cubic {
namespace slang {

void Attribute::WriteTo(std::string& source) {
  source +=
      fmt::format("layout(location = {}) {} {} {};\n", mLocation, mIsInput ? "in" : "out", GetTypeString(), mName);
}

const char* Attribute::GetTypeString() const {
  switch (mFormat) {
    case VertexFormat::kFloat32:
      return "float";
    case VertexFormat::kFloat32x2:
      return "vec2";
    case VertexFormat::kFloat32x3:
      return "vec3";
    case VertexFormat::kFloat32x4:
      return "vec4";
  }
}

std::string Attribute::GenExpression() const {
  if (mFormat == VertexFormat::kFloat32) {
    return fmt::format("vec4({})", mName);
  } else if (mFormat == VertexFormat::kFloat32x2) {
    return fmt::format("vec4({}, 0.0, 1.0)", mName);
  } else if (mFormat == VertexFormat::kFloat32x3) {
    return fmt::format("vec4({}, 1.0)", mName);
  } else {
    return mName;
  }

  return "";
}

}  // namespace slang
}  // namespace cubic
