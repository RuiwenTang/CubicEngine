#pragma once

#include <cubic/render/types.h>

#include <optional>
#include <vector>

#include "slang/node.h"

namespace cubic {
namespace slang {

class Function : public Node {
 public:
  Function(std::string name) : mName(std::move(name)) {}

  Function(std::string name, VertexFormat retType) : mName(std::move(name)), mRetType(retType) {}

  ~Function() override = default;

  void WriteTo(std::string &source) override;

  const char *GetName() const override { return mName.c_str(); }

 protected:
  virtual std::string GenFunctionBody() = 0;

 private:
  std::string mName;
  std::optional<VertexFormat> mRetType = {};
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

}  // namespace slang
}  // namespace cubic
