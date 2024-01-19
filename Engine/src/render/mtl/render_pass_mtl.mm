#include "render/mtl/render_pass_mtl.h"

namespace cubic {

RenderPassMTL::RenderPassMTL(id<MTLRenderCommandEncoder> encoder) : mEncoder(encoder) {}

RenderPassMTL::~RenderPassMTL() { [mEncoder release]; }

}
