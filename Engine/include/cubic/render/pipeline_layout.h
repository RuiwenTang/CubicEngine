#pragma once

#include <cubic/platform.h>
#include <cubic/render/bind_group.h>

#include <memory>
#include <vector>

namespace cubic {

class CUBIC_API PipelineLayout {
 public:
  PipelineLayout(std::vector<std::shared_ptr<BindGroupLayout>> groups) : mGroups(std::move(groups)) {}

  virtual ~PipelineLayout() = default;

  std::shared_ptr<BindGroupLayout> GetGroup(uint32_t index) {
    if (index < mGroups.size()) {
      return mGroups[index];
    }

    return {};
  }

 private:
  std::vector<std::shared_ptr<BindGroupLayout>> mGroups;
};

}  // namespace cubic
