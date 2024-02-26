#include "slang/program.h"

#include "slang/attribute.h"
#include "slang/function.h"

namespace cubic {
namespace slang {

constexpr const char* GLS_VERSION_HEADER = "#version 450 core";

void Program::WriteTo(std::string& source) {
  source += GLS_VERSION_HEADER;

  source += "\n";
  source += "\n";

  // write all global scope object
  for (auto& node : mGlobalScope) {
    node->WriteTo(source);
  }

  // write main function

  if (mMainFunc) {
    mMainFunc->WriteTo(source);
  }
}

}  // namespace slang
}  // namespace cubic
