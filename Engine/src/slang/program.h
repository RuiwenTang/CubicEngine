#pragma once

#include <vector>

#include "slang/node.h"

namespace cubic {
namespace slang {

class Attribute;
class Function;

class Program : public Node {
 public:
  Program() = default;
  ~Program() override = default;

  void WriteTo(std::string& source) override;

  void AddToGlobalScope(Node* node) { mGlobalScope.emplace_back(node); }

  void SetMainFunc(Function* func) { mMainFunc = func; }

 private:
  std::vector<Node*> mGlobalScope = {};

  Function* mMainFunc = {};
};

}  // namespace slang
}  // namespace cubic
