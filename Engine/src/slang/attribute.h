#pragma once

#include "slang/node.h"
#include "slang/types.h"

namespace cubic {
namespace slang {

class Attribute : public Node {
 public:
  Attribute(uint32_t location, ScalarType type, std::string name, bool isInput)
      : mLocation(location), mType(type), mName(std::move(name)), mIsInput(isInput) {}

  ~Attribute() = default;

  void WriteTo(std::string& source) override;

  const char* GetName() const override { return mName.c_str(); }

  ScalarType GetScalarType() const { return mType; }

  uint32_t GetLocation() const { return mLocation; }

 private:
  uint32_t mLocation;
  ScalarType mType;
  std::string mName;
  bool mIsInput;
};

}  // namespace slang
}  // namespace cubic
