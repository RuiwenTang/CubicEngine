#include "slang/types.h"

namespace cubic {
namespace slang {

ScalarType ConvertScalarType(VertexFormat format) {
  switch (format) {
    case VertexFormat::kFloat32:
      return ScalarType::kFloat;
    case VertexFormat::kFloat32x2:
      return ScalarType::kVec2;
    case VertexFormat::kFloat32x3:
      return ScalarType::kVec3;
    case VertexFormat::kFloat32x4:
      return ScalarType::kVec4;
  }

  return ScalarType::kFloat;
}

const char* ScalarTypeString(ScalarType type) {
  switch (type) {
    case ScalarType::kFloat:
      return "float";
    case ScalarType::kVec2:
      return "vec2";
    case ScalarType::kVec3:
      return "vec3";
    case ScalarType::kVec4:
      return "vec4";
    case ScalarType::kMat4:
      return "mat4";
  }
}

}  // namespace slang
}  // namespace cubic