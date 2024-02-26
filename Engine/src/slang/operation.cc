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

void BinaryOperation::WriteTo(std::string& source) {
  source += fmt::format("{} {} {}", mLeft->GetName(), operator_to_string(mOp), mRight->GenExpression());
}

std::string BinaryOperation::GenExpression() const {
  return fmt::format("{} {} {}", mLeft->GetName(), operator_to_string(mOp), mRight->GenExpression());
}

}  // namespace slang
}  // namespace cubic
