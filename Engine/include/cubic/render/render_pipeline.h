#pragma once

#include <cubic/platform.h>
#include <cubic/render/shader_module.h>
#include <cubic/render/types.h>

#include <memory>

namespace cubic {

struct ColorTargetState {
  TextureFormat format = TextureFormat::kRGBA8Unorm;
  ColorWriteMask writeMask = ColorWriteMask::kAll;
  BlendState* blend = nullptr;
};

struct DepthStencilState {
  TextureFormat format = TextureFormat::kDepth24;
  bool depthWriteEnable = false;
  CompareFunction depthCompare = CompareFunction::kNever;
  StencilFaceState stencilFront = {};
  StencilFaceState stencilBack = {};
  uint32_t stencilReadMask;
  uint32_t stencilWriteMask;
};

struct RenderPipelineDescriptor {
  std::shared_ptr<ShaderModule> vertexShader = {};
  std::shared_ptr<ShaderModule> fragmentShader = {};
  DepthStencilState* depthStencil = {};
  uint32_t colorCount = 0;
  ColorTargetState* pColorTargets = {};
};

class CUBIC_API RenderPipeline {
 public:
  virtual ~RenderPipeline() = default;
};

}  // namespace cubic
