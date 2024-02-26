#pragma once

#include <vector>

#include "slang/node.h"

namespace cubic {
namespace slang {

class Statement : public Node {
 public:
  Statement() = default;

  ~Statement() override = default;

  void AddExpression(Node *exp) { mExpresions.emplace_back(exp); }

  void WriteTo(std::string &source) override;

 private:
  std::vector<Node *> mExpresions = {};
};

}  // namespace slang
}  // namespace cubic
