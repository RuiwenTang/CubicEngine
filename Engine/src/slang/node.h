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
