#include "slang/function.h"

#include <fmt/core.h>

namespace cubic {
namespace slang {

std::string get_ret_type(const std::optional<ScalarType>& format) {
  if (!format.has_value()) {
    return "void";
  }

  return ScalarTypeString(format.value());
}

void Function::WriteTo(std::string& source) {
  source += "\n";

  std::string func = fmt::format("{} {} ()", get_ret_type(mRetType), mName);

  func += "{\n";

  func += GenFunctionBody();

  func += "\n}\n";

  source += func;
}

std::string StatementFunction::GenFunctionBody() {
  std::string body = {};

  for (const auto& statement : mStatements) {
    statement->WriteTo(body);
  }

  return body;
}

}  // namespace slang
}  // namespace cubic
