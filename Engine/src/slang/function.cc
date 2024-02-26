#include "slang/function.h"

#include <fmt/core.h>

namespace cubic {
namespace slang {

std::string get_ret_type(const std::optional<VertexFormat>& format) {
  if (!format.has_value()) {
    return "void";
  }

  switch (format.value()) {
    case VertexFormat::kFloat32:
      return "float";
    case VertexFormat::kFloat32x2:
      return "vec2";
    case VertexFormat::kFloat32x3:
      return "vec3";
    case VertexFormat::kFloat32x4:
      return "vec4";
  }
}

void Function::WriteTo(std::string& source) {
  source += "\n";

  std::string func = fmt::format("{} {} ()", get_ret_type(mRetType), mName);

  func += "{\n";

  func += GenFunctionBody();

  func += "}\n";

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
