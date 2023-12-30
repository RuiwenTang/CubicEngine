#pragma once

#include <cubic/render/render_system.h>

#include <memory>

namespace cubic {

std::unique_ptr<RenderSystem> InitRenderSystem();

}
