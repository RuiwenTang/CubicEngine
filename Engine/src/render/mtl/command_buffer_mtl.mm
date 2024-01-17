#include "render/mtl/command_buffer_mtl.h"

namespace cubic {

CommandBufferMTL::CommandBufferMTL(id<MTLCommandBuffer> cmd) : mCMD(cmd) {}

CommandBufferMTL::~CommandBufferMTL() { [mCMD release]; }

std::unique_ptr<RenderPass> CommandBufferMTL::BeginRenderPass(const RenderPassDescriptor &desc) { return {}; }

void CommandBufferMTL::EndRenderPass(std::unique_ptr<RenderPass> render_pass) {}

}