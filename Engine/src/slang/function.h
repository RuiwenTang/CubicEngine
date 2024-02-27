#pragma once

#include <optional>
#include <vector>

#include "slang/node.h"
#include "slang/types.h"

namespace cubic {
namespace slang {

class Function : public Node {
 public:
  Function(std::string name) : mName(std::move(name)) {}

  Function(std::string name, ScalarType retType) : mName(std::move(name)), mRetType(retType) {}

  ~Function() override = default;

  void WriteTo(std::string &source) override;

  const char *GetName() const override { return mName.c_str(); }

 protected:
  virtual std::string GenFunctionBody() = 0;

 private:
  std::string mName;
  std::optional<ScalarType> mRetType = {};
};

class StatementFunction : public Function {
 public:
  StatementFunction(std::string name) : Function(std::move(name)) {}

  ~StatementFunction() = default;

  void AddStatement(Node *statement) { mStatements.emplace_back(statement); }

 protected:
  std::string GenFunctionBody() override;

 private:
  std::vector<Node *> mStatements = {};
};

class StringFunction : public Function {
 public:
  StringFunction(std::string name, std::string content)
      : Function(std::move(name), ScalarType::kVec4), mContent(std::move(content)) {}

  ~StringFunction() = default;

 protected:
  std::string GenFunctionBody() override { return mContent; }

 private:
  std::string mContent;
};

}  // namespace slang
}  // namespace cubic
