#pragma once

#include "render/render_system_priv.h"

// metal header
#import <Metal/Metal.h>

namespace cubic {

struct RenderSystemInfoMTL : public RenderSystemInfo {
  id<MTLDevice> device = nil;
  id<MTLCommandQueue> queue = nil;
};

}  // namespace cubic
