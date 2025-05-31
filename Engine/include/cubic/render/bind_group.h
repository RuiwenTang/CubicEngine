#pragma once

#include <cubic/platform.h>
#include <cubic/render/buffer.h>
#include <cubic/render/texture.h>
#include <cubic/render/types.h>
#include <cubic/render/sampler.h>

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

using BindResource = std::variant<BufferView, std::shared_ptr<Texture>, Sampler>;

struct GroupEntry {
  uint32_t binding = 0;
  EntryType type = EntryType::kNone;
  BindResource resource = {};
};

class CUBIC_API BindGroup final {
 public:
  explicit BindGroup(uint32_t index) : mIndex(index) {}

  ~BindGroup() = default;

  void AddBinding(uint32_t binding, EntryType type, const BindResource& resource) {
    mEntries.push_back({binding, type, resource});
  }

  uint32_t GetIndex() const { return mIndex; }

  const std::vector<GroupEntry>& GetEntries() const { return mEntries; }

  const GroupEntry* GetEntry(uint32_t binding) const {
    for (const auto& entry : mEntries) {
      if (entry.binding == binding) {
        return &entry;
      }
    }

    return nullptr;
  }

 private:
  uint32_t mIndex;
  std::vector<GroupEntry> mEntries;
};

}  // namespace cubic
