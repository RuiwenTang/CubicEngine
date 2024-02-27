#include "slang/operation.h"

#include <fmt/core.h>

namespace cubic {
namespace slang {

const char* operator_to_string(Operator op) {
  switch (op) {
    case Operator::kEqual:
      return "=";
    case Operator::kMultiply:
      return "*";
  }
}

void TypeConvertOperation::WriteTo(std::string& source) {
  if (mTargetType == mNodeType) {
    source += mNode->GetName();
  } else {
    WriteTypeConvert(source);
  }
}

void TypeConvertOperation::WriteTypeConvert(std::string& source) {
  if (mNodeType == ScalarType::kFloat) {
    source += fmt::format("{}( {} )", ScalarTypeString(mTargetType), mNode->GetName());
  } else if (mTargetType == ScalarType::kVec2) {
    source += fmt::format("vec2({0}.x, {0}.y)", mNode->GetName());
  } else if (mTargetType == ScalarType::kVec3) {
    if (mNodeType == ScalarType::kVec2) {
      source += fmt::format("vec3({0}.x, {0}.y, 0.0)", mNode->GetName());
    } else {
      source += fmt::format("vec3({0}.x, {0}.y, {0}.z)", mNode->GetName());
    }
  } else {
    if (mNodeType == ScalarType::kVec2) {
      source += fmt::format("vec4({}, 0.0, 1.0)", mNode->GetName());
    } else {
      source += fmt::format("vec4({}, 1.0)", mNode->GetName());
    }
  }
}

void BinaryOperation::WriteTo(std::string& source) {
  std::string right_str{};

  mRight->WriteTo(right_str);

  source += fmt::format("{} {} {}", mLeft->GetName(), operator_to_string(mOp), right_str);
}

void CallOperation::WriteTo(std::string& source) { source += fmt::format("{}()", mNode->GetName()); }

}  // namespace slang
}  // namespace cubic
