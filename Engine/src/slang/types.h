#pragma once

#include <cubic/render/types.h>

namespace cubic {
namespace slang {

enum class ScalarType {
  kFloat = 1,
  kVec2,
  kVec3,
  kVec4,
  kMat4,
};

ScalarType ConvertScalarType(VertexFormat format);

const char* ScalarTypeString(ScalarType type);

}  // namespace slang
}  // namespace cubic