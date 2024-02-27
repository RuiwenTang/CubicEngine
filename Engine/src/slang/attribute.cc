#include "slang/attribute.h"

#include <fmt/core.h>

namespace cubic {
namespace slang {

void Attribute::WriteTo(std::string& source) {
  source += fmt::format("layout(location = {}) {} {} {};\n", mLocation, mIsInput ? "in" : "out",
                        ScalarTypeString(mType), mName);
}

}  // namespace slang
}  // namespace cubic
