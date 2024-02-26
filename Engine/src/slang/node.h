#pragma once

#include <memory>
#include <string>
#include <vector>

namespace cubic {
namespace slang {

class Node {
 public:
  virtual ~Node() = default;

  virtual void WriteTo(std::string& source) = 0;

  virtual const char* GetName() const { return ""; }

  virtual std::string GenExpression() const { return ""; }
};

class NamedNode : public Node {
 public:
  NamedNode(const char* name) : mName(name) {}

  ~NamedNode() override = default;

  void WriteTo(std::string& source) override { source += mName; }

  const char* GetName() const override { return mName; }

  std::string GenExpression() const override { return mName; }

 private:
  const char* mName;
};

class NodeHeap {
 public:
  NodeHeap() = default;
  ~NodeHeap() = default;

  template <class T, class... Args>
  T* Allocate(Args&&... args) {
    mNodes.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));

    return static_cast<T*>(mNodes.back().get());
  }

 private:
  std::vector<std::unique_ptr<Node>> mNodes = {};
};

}  // namespace slang
}  // namespace cubic
