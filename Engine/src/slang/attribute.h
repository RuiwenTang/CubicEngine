#pragma once

#include <cubic/render/types.h>

#include "slang/node.h"

namespace cubic {
namespace slang {

class Attribute : public Node {
 public:
  Attribute(uint32_t location, VertexFormat format, std::string name, bool isInput)
      : mLocation(location), mFormat(format), mName(std::move(name)), mIsInput(isInput) {}

  ~Attribute() = default;

  void WriteTo(std::string& source) override;

  const char* GetName() const override { return mName.c_str(); }

 private:
  const char* GetTypeString() const;

 private:
  uint32_t mLocation;
  VertexFormat mFormat;
  std::string mName;
  bool mIsInput;
};

}  // namespace slang
}  // namespace cubic
