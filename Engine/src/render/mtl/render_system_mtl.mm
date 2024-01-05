#include "render/mtl/render_system_mtl.h"

#import <Metal/Metal.h>
#include "cubic/render/render_system.h"
#include "render/mtl/render_system_info_mtl.h"
#include "render/render_system_priv.h"

namespace cubic {

class RenderSystemMTLPriv {
 public:
  RenderSystemMTLPriv() = default;

  ~RenderSystemMTLPriv() = default;

  bool Init() {
    mDevice = MTLCreateSystemDefaultDevice();
    mQueue = [mDevice newCommandQueue];

    mInfo.backend = Backend::kMetal;
    mInfo.device = mDevice;
    mInfo.queue = mQueue;

    return mDevice != nil;
  }

  RenderSystemInfo* GetBackendInfo() { return &mInfo; }

 private:
  id<MTLDevice> mDevice = nil;
  id<MTLCommandQueue> mQueue = nil;

  RenderSystemInfoMTL mInfo = {};
};

std::unique_ptr<RenderSystemMTL> RenderSystemMTL::Create() { return std::make_unique<RenderSystemMTL>(); }

RenderSystemMTL::RenderSystemMTL() {}

RenderSystemMTL::~RenderSystemMTL() {}

bool RenderSystemMTL::Init() {
  mPriv = std::make_unique<RenderSystemMTLPriv>();

  return mPriv->Init();
}

RenderSystemInfo* RenderSystemMTL::GetBackendInfo() { return mPriv->GetBackendInfo(); }

}
