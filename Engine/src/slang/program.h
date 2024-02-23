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

  void AddInput(Attribute* attr);
  void AddOutput(Attribute* attr);

  void SetMainFunc(Function* func) { mMainFunc = func; }

 private:
  std::vector<Attribute*> mInputAttrs = {};
  std::vector<Attribute*> mOutputAttrs = {};

  Function* mMainFunc = {};
};

}  // namespace slang
}  // namespace cubic
