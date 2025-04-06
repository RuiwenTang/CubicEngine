#pragma once

#include <cubic/platform.h>
#include <cubic/render/bind_group.h>

namespace cubic {

class BindGroupLayout;

class CUBIC_API PipelineLayout {
  friend class RenderSystemPriv;

 public:
  PipelineLayout() {}

  virtual ~PipelineLayout() = default;

  virtual const BindGroupLayout* GetBindGroupLayout(uint32_t slot) const = 0;
};

}  // namespace cubic
