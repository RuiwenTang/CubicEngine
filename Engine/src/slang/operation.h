#pragma once

#include "slang/node.h"
#include "slang/types.h"

namespace cubic {
namespace slang {

enum class Operator {
  kEqual,
  kMultiply,
};

class TypeConvertOperation : public Node {
 public:
  TypeConvertOperation(ScalarType targetType, ScalarType nodeType, Node* node, bool geneous = false)
      : mTargetType(targetType), mNodeType(nodeType), mNode(node), mGeneous(geneous) {}

  ~TypeConvertOperation() override = default;

  void WriteTo(std::string& source) override;

 private:
  void WriteTypeConvert(std::string& source);

 private:
  ScalarType mTargetType;
  ScalarType mNodeType;
  Node* mNode;
  bool mGeneous;
};

class BinaryOperation : public Node {
 public:
  BinaryOperation(Operator op, Node* left, Node* right) : mOp(op), mLeft(left), mRight(right) {}

  ~BinaryOperation() override = default;

  void WriteTo(std::string& source) override;

 private:
  Operator mOp;
  Node* mLeft;
  Node* mRight;
};

class CallOperation : public Node {
 public:
  CallOperation(Node* node) : mNode(node) {}

  ~CallOperation() override = default;

  void WriteTo(std::string& source) override;

 private:
  Node* mNode;
};

class Declaration : public Node {
 public:
  Declaration(const char* name, ScalarType type, Node* initializer)
      : mName(name), mType(type), mInitializer(initializer) {}

  ~Declaration() override = default;

  void WriteTo(std::string& source) override;

  const char* GetName() const { return mName; }

 private:
  const char* mName;
  ScalarType mType;
  Node* mInitializer = nullptr;
};

}  // namespace slang
}  // namespace cubic
