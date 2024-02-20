#pragma once

#include <cubic/platform.h>
#include <cubic/render/pipeline_layout.h>
#include <cubic/render/shader_module.h>
#include <cubic/render/types.h>

#include <memory>
#include <vector>

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
  std::vector<VertexBufferLayout> vertexBuffer = {};
  std::shared_ptr<ShaderModule> vertexShader = {};
  std::shared_ptr<ShaderModule> fragmentShader = {};
  std::shared_ptr<PipelineLayout> layout = {};
  DepthStencilState* depthStencil = {};
  uint32_t colorCount = 0;
  ColorTargetState* pColorTargets = {};
  uint32_t sampleCount = 1;
};

class CUBIC_API RenderPipeline {
 public:
  RenderPipeline(std::shared_ptr<PipelineLayout> layout) : mLayout(std::move(layout)) {}

  virtual ~RenderPipeline() = default;

  const std::shared_ptr<PipelineLayout>& GetLayout() const { return mLayout; }

 private:
  std::shared_ptr<PipelineLayout> mLayout;
};

}  // namespace cubic
