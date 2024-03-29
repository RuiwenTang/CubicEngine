#pragma once

#include <cubic/platform.h>
#include <cubic/render/bind_group.h>
#include <cubic/render/types.h>

#include <string>
#include <vector>

namespace cubic {

struct MaterialInput {
  std::string name = {};

  VertexFormat format = VertexFormat::kFloat32;
};

enum class ResourceType {
  kUniform,
  kTexture,
};

struct ResourceData {
  void* data = nullptr;
  uint64_t length = 0;
};

class CUBIC_API Material {
 public:
  virtual ~Material() = default;

  virtual std::vector<MaterialInput> GetInput() const = 0;

  virtual std::vector<GroupEntryInfo> GetResourceInfo() const = 0;

  virtual std::vector<ResourceData> GetResourceData() const = 0;

  virtual std::string GenResourceSet(uint32_t index) const = 0;

  virtual std::string GenColorFunction() const = 0;
};

}  // namespace cubic
