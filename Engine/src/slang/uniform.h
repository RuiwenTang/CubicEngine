#pragma once

#include <vector>

#include "slang/node.h"
#include "slang/types.h"

namespace cubic {
namespace slang {

class UniformSet;

class UniformBinding : public Node {
 public:
  UniformBinding(ScalarType type, std::string name) : mType(type), mName(std::move(name)) {}
  ~UniformBinding() override = default;

  const char* GetName() const override;

  void WriteTo(std::string& source) override;

  void SetParent(UniformSet* set);

 private:
  ScalarType mType;
  std::string mName;

  std::string mCallName;

  UniformSet* mSet;
};

class UniformSet : public Node {
 public:
  UniformSet(uint32_t setIndex, uint32_t bindingIndex, std::string name)
      : mSetIndex(setIndex), mBindingIndex(bindingIndex), mName(std::move(name)) {}

  void AddBinding(UniformBinding* binding);

  const char* GetName() const override { return mName.c_str(); }

  void WriteTo(std::string& source) override;

 private:
  uint32_t mSetIndex;
  uint32_t mBindingIndex;
  std::string mName;

  std::vector<UniformBinding*> mBindings = {};
};

class StringUniformSet : public Node {
 public:
  StringUniformSet(std::string content) : mContent(std::move(content)) {}

  ~StringUniformSet() override = default;

  const char* GetName() const override { return ""; }

  void WriteTo(std::string& source) override { source += mContent; }

 private:
  std::string mContent;
};

}  // namespace slang
}  // namespace cubic