#pragma once

#include <cubic/render/types.h>

#include <optional>

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

 protected:
  std::string GenFunctionBody() override;
};

}  // namespace slang
}  // namespace cubic
