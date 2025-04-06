#pragma once

#include <cubic/platform.h>
#include <cubic/render/buffer.h>
#include <cubic/render/texture.h>
#include <cubic/render/types.h>

#include <variant>
#include <vector>

namespace cubic {

enum class EntryType {
  kNone,
  kUniformBuffer,
  kStorgeBuffer,
  kTexture,
  kSampler,
};

struct GroupEntryInfo {
  uint32_t binding = 0;
  EntryType type = EntryType::kNone;
  ShaderStageMask visibility = 0;
};

using BindResource = std::variant<BufferView, std::shared_ptr<Texture>>;

struct GroupEntry {
  uint32_t binding = 0;
  EntryType type = EntryType::kNone;
  BindResource resource = {};
};

class CUBIC_API BindGroup {
 public:
  virtual ~BindGroup() = default;
};

}  // namespace cubic
