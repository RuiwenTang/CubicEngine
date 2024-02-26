#pragma once

#include "slang/node.h"

namespace cubic {
namespace slang {

enum class Operator {
  kEqual,
  kMultiply,
};

class BinaryOperation : public Node {
 public:
  BinaryOperation(Operator op, Node* left, Node* right) : mOp(op), mLeft(left), mRight(right) {}

  ~BinaryOperation() override = default;

  void WriteTo(std::string& source) override;

  std::string GenExpression() const override;

 private:
  Operator mOp;
  Node* mLeft;
  Node* mRight;
};

}  // namespace slang
}  // namespace cubic
