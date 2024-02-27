#include "slang/statement.h"

namespace cubic {
namespace slang {

void Statement::WriteTo(std::string& source) {
  if (mExpresions.empty()) {
    return;
  }

  for (const auto& exp : mExpresions) {
    exp->WriteTo(source);
    source += " ";
  }

  source += ";\n";
}

}  // namespace slang
}  // namespace cubic
