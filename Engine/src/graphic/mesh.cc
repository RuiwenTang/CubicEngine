#include <cubic/graphic/mesh.h>

#include "graphic/shader_generator.h"

namespace cubic {

std::shared_ptr<ShaderModule> gen_shader(ShaderStage stage, const std::string& code, RenderSystem* renderSystem) {
  ShaderModuleDescriptor desc{};

  desc.stage = stage;
  desc.code = code;
  desc.label = "mesh shader";

  return renderSystem->CreateShaderModule(&desc);
}

Mesh::Mesh(std::shared_ptr<Geometry> geom, std::shared_ptr<Material> material)
    : mGeometry(std::move(geom)), mMaterial(std::move(material)) {}

bool Mesh::Prepare(RenderSystem* renderSystem, TextureFormat targetFormat) {
  if (!PreparePipeline(renderSystem, targetFormat)) {
    return false;
  }

  return false;
}

void Mesh::Draw(RenderPass* renderPass) {}

bool Mesh::PreparePipeline(RenderSystem* renderSystem, TextureFormat targetFormat) {
  if (mRenderPipeline) {
    return true;
  }

  ShaderGenerator sg{mGeometry.get(), mMaterial.get()};

  if (!sg.Prepare()) {
    return false;
  }

  auto vs = gen_shader(ShaderStage::kVertexShader, sg.GenVertexShader(), renderSystem);
  auto fs = gen_shader(ShaderStage::kFragmentShader, sg.GenFragmentShader(), renderSystem);

  if (!vs || !fs) {
    return false;
  }

  RenderPipelineDescriptor desc{};

  desc.vertexShader = vs;
  desc.fragmentShader = fs;
  desc.vertexBuffer = sg.GenVertexBufferLayout();

  return false;
}

}  // namespace cubic
