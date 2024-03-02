#pragma once

#include <cubic/graphic/geometry.h>
#include <cubic/graphic/material.h>
#include <cubic/render/pipeline_layout.h>
#include <cubic/render/types.h>

#include <vector>

#include "slang/attribute.h"
#include "slang/program.h"

namespace cubic {

class RenderSystem;

class ShaderGenerator {
 public:
  ShaderGenerator(Geometry* geometry, Material* material) : mGeometry(geometry), mMaterial(material) {}

  ~ShaderGenerator() = default;

  bool Prepare();

  std::string GenVertexShader();

  std::string GenFragmentShader();

  std::vector<VertexBufferLayout> GenVertexBufferLayout();

  std::shared_ptr<PipelineLayout> GenPipelineLayout(RenderSystem* renderSystem);

 private:
  void BuildVertexProgram();

  void BuildFragmentProgram();

 private:
  Geometry* mGeometry;
  Material* mMaterial;

  slang::NodeHeap mHeap = {};

  std::vector<slang::Attribute*> mVertexInputState = {};
  std::vector<slang::Attribute*> mStageShareState = {};
  std::vector<slang::Node*> mStageShareStatement = {};

  slang::Attribute* mPosAttribute = {};

  slang::Program mVertexShader = {};
  slang::Program mFragmentShader = {};
};

}  // namespace cubic