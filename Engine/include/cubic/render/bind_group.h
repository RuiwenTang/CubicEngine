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
  kCombinedImageSampler,
  kTexture,
  kSampler,
};

struct GroupEntryInfo {
  uint32_t binding = 0;
  EntryType type = EntryType::kNone;
  ShaderStageMask visibility = 0;
};

class BindGroupLayout {
 public:
  explicit BindGroupLayout(uint32_t index) : mIndex(index) {}

  BindGroupLayout(uint32_t index, std::vector<GroupEntryInfo> entries) : mIndex(index), mEntries(std::move(entries)) {}

  virtual ~BindGroupLayout() = default;

  uint32_t GetIndex() const { return mIndex; }

  const std::vector<GroupEntryInfo>& GetEntries() const { return mEntries; }

  std::vector<GroupEntryInfo>& GetEntries() { return mEntries; }

  void AddBinding(uint32_t binding, EntryType type, ShaderStageMask visibility) {
    mEntries.push_back({binding, type, visibility});
  }

 private:
  uint32_t mIndex = 0;  // Bind group index
  std::vector<GroupEntryInfo> mEntries;
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
