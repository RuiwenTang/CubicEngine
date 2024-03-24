#include "render/render_system_priv.h"

#include <cubic/core/log.h>

#include "render/shader_compiler.h"

#if defined(CUBIC_PLATFORM_WINDOWS) || defined(CUBIC_PLATFORM_LINUX)

#include "render/vk/render_system_vk.h"

#elif defined(CUBIC_PLATFORM_MACOS)

#include "render/mtl/render_system_mtl.h"

#endif

namespace cubic {

std::unique_ptr<RenderSystem> RenderSystemPriv::InitRenderSystem(bool enableDebug) {
#if defined(CUBIC_PLATFORM_WINDOWS) || defined(CUBIC_PLATFORM_LINUX)

  CUB_INFO("Init Vulkan backend...");

  auto render_system = RenderSystemVk::Create();

  if (!render_system->Init(enableDebug)) {
    CUB_ERROR("Failed init vulkan backend context !!");

    return std::unique_ptr<RenderSystem>();
  }

  return render_system;
#elif defined(CUBIC_PLATFORM_MACOS)

  CUB_INFO("Init Metal backend...");

  auto render_system = RenderSystemMTL::Create();

  if (!render_system->Init()) {
    CUB_ERROR("Failed init Metal backend context !!");

    return std::unique_ptr<RenderSystem>();
  }

  return render_system;
#endif

  return std::unique_ptr<RenderSystem>();
}

std::shared_ptr<ShaderModule> RenderSystemPriv::CreateShaderModule(ShaderModuleDescriptor* desc) {
  ShaderCompiler compiler{};
  compiler.SetTargetStage(desc->stage);

  auto result = compiler.Compile(desc->label, desc->code);

  if (result.empty()) {
    return {};
  }

  return CompileBackendShader(desc, result);
}

std::shared_ptr<BindGroupLayout> RenderSystemPriv::CreateBindGroupLayout(std::vector<GroupEntryInfo> entries) {
  return std::make_shared<BindGroupLayout>(std::move(entries));
}

std::shared_ptr<PipelineLayout> RenderSystemPriv::CreatePipelineLayout(
    std::vector<std::shared_ptr<BindGroupLayout>> groups) {
  return std::make_shared<PipelineLayout>(std::move(groups));
}

}  // namespace cubic
