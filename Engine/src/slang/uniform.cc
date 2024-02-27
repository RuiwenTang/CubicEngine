#include "slang/uniform.h"

#include <fmt/core.h>

namespace cubic {
namespace slang {

const char* UniformBinding::GetName() const { return mCallName.c_str(); }

void UniformBinding::WriteTo(std::string& source) { source += fmt::format("{} {};\n", ScalarTypeString(mType), mName); }

void UniformBinding::SetParent(UniformSet* set) {
  mSet = set;

  mCallName = fmt::format("{}.{}", mSet->GetName(), mName);
}

void UniformSet::AddBinding(UniformBinding* binding) {
  binding->SetParent(this);

  mBindings.emplace_back(binding);
}

void UniformSet::WriteTo(std::string& source) {
  source += fmt::format("layout(set = {}, binding = {}) uniform Def{} {{\n", mSetIndex, mBindingIndex, mName);

  for (auto binding : mBindings) {
    binding->WriteTo(source);
  }

  source += "\n";
  source += fmt::format("}} {} ;\n", mName);
}

}  // namespace slang
}  // namespace cubic
