#include "render/mtl/render_system_mtl.h"

#import <Metal/Metal.h>
#include "cubic/render/render_system.h"
#include "render/mtl/command_queue_mtl.h"
#include "render/mtl/render_pipeline_mtl.h"
#include "render/mtl/render_system_info_mtl.h"
#include "render/mtl/shader_module_mtl.h"
#include "render/mtl/texture_mtl.h"
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

    mQueueProxy = std::make_unique<CommandQueueMTL>(mQueue);

    return mDevice != nil;
  }

  RenderSystemInfo* GetBackendInfo() { return &mInfo; }

  CommandQueueMTL* GetQueueProxy() { return mQueueProxy.get(); }

  id<MTLDevice> GetNativeGPU() const { return mDevice; }

 private:
  id<MTLDevice> mDevice = nil;
  id<MTLCommandQueue> mQueue = nil;

  RenderSystemInfoMTL mInfo = {};
  std::unique_ptr<CommandQueueMTL> mQueueProxy = {};
};

std::unique_ptr<RenderSystemMTL> RenderSystemMTL::Create() { return std::make_unique<RenderSystemMTL>(); }

RenderSystemMTL::RenderSystemMTL() {}

RenderSystemMTL::~RenderSystemMTL() {}

bool RenderSystemMTL::Init() {
  mPriv = std::make_unique<RenderSystemMTLPriv>();

  return mPriv->Init();
}

RenderSystemInfo* RenderSystemMTL::GetBackendInfo() { return mPriv->GetBackendInfo(); }

CommandQueue* RenderSystemMTL::GetCommandQueue(QueueType type) { return mPriv->GetQueueProxy(); }

std::shared_ptr<RenderPipeline> RenderSystemMTL::CreateRenderPipeline(RenderPipelineDescriptor* desc) {
  return RenderPipelineMTL::Create(desc, mPriv->GetNativeGPU());
}

std::shared_ptr<Texture> RenderSystemMTL::CreateTexture(TextureDescirptor* desc) {
  return TextureMTL::Create(desc, mPriv->GetNativeGPU());
}

std::shared_ptr<ShaderModule> RenderSystemMTL::CompileBackendShader(ShaderModuleDescriptor* desc,
                                                                    const std::vector<uint32_t>& spv) {
  return ShaderModuleMTL::Compile(mPriv->GetNativeGPU(), desc, spv);
}

}  // namespace cubic
