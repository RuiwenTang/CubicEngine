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

  // write all inputs
  for (auto attr : mInputAttrs) {
    attr->WriteTo(source);
  }

  // write all outputs
  for (auto attr : mOutputAttrs) {
    attr->WriteTo(source);
  }

  // write main function

  if (mMainFunc) {
    mMainFunc->WriteTo(source);
  }
}

void Program::AddInput(Attribute* attr) { mInputAttrs.emplace_back(attr); }

void Program::AddOutput(Attribute* attr) { mOutputAttrs.emplace_back(attr); }

}  // namespace slang
}  // namespace cubic
