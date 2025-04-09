#pragma once

#include <cubic/platform.h>
#include <cubic/render/bind_group.h>

#include <vector>

namespace cubic {

class CUBIC_API PipelineLayout {
  friend class RenderSystemPriv;

 public:
  PipelineLayout() {}

  virtual ~PipelineLayout() = default;
};

}  // namespace cubic
