#include "render/mtl/render_system_mtl.h"

#import <Metal/Metal.h>

namespace cubic {

class RenderSystemMTLPriv {
 public:
  RenderSystemMTLPriv() = default;

  ~RenderSystemMTLPriv() = default;

  bool Init() {
    mDevice = MTLCreateSystemDefaultDevice();

    return mDevice != nil;
  }

 private:
  id<MTLDevice> mDevice = nil;
};

std::unique_ptr<RenderSystemMTL> RenderSystemMTL::Create() { return std::make_unique<RenderSystemMTL>(); }

RenderSystemMTL::RenderSystemMTL() {}

RenderSystemMTL::~RenderSystemMTL() {}

bool RenderSystemMTL::Init() {
  mPriv = std::make_unique<RenderSystemMTLPriv>();

  return mPriv->Init();
}

}
