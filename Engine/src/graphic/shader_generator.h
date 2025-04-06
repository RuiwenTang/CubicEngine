#pragma once

#include <cubic/graphic/geometry.h>
#include <cubic/graphic/material.h>
#include <cubic/render/pipeline_layout.h>
#include <cubic/render/types.h>

#include <vector>

namespace cubic {

class RenderSystem;

class ShaderGenerator {
 public:
  ShaderGenerator(Geometry* geometry, Material* material) : mGeometry(geometry), mMaterial(material) {}

  ~ShaderGenerator() = default;

  bool Prepare();

  std::string GenVertexShader();

  std::string GenFragmentShader();

 private:
  void BuildVertexProgram();

  void BuildFragmentProgram();

 private:
  Geometry* mGeometry;
  Material* mMaterial;
};

}  // namespace cubic