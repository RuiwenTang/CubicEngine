#pragma once

#include <cubic/render/pipeline_layout.h>

#include "render/mtl/shader_module_mtl.h"

namespace cubic {

class PipelineLayoutMTL : public PipelineLayout {
 public:
  PipelineLayoutMTL(std::vector<StageGroup> vs_group, std::vector<StageGroup> fs_group)
      : mVSGroups(std::move(vs_group)), mFSGroups(std::move(fs_group)) {}

  PipelineLayoutMTL(PipelineLayoutMTL&&) = default;

  PipelineLayoutMTL(const PipelineLayoutMTL&) = default;

  ~PipelineLayoutMTL() override = default;

  void SetBindGroup(id<MTLRenderCommandEncoder> encoder, uint32_t slot, const BindGroup& group) const;

 private:
  std::vector<StageGroup> mVSGroups = {};
  std::vector<StageGroup> mFSGroups = {};
};

}  // namespace cubic
