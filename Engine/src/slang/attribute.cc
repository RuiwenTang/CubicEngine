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

}  // namespace slang
}  // namespace cubic
