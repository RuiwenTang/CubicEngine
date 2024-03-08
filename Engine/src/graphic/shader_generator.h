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

struct ShaderConfigration {
  bool has_model_matrix = false;
};

class ShaderGenerator {
 public:
  ShaderGenerator(Geometry* geometry, Material* material) : mGeometry(geometry), mMaterial(material) {}

  ~ShaderGenerator() = default;

  ShaderConfigration& GetConfigration() { return mConfigration; }

  bool Prepare();

  std::string GenVertexShader();

  std::string GenFragmentShader();

  std::vector<VertexBufferLayout> GenVertexBufferLayout();

  std::shared_ptr<PipelineLayout> GenPipelineLayout(RenderSystem* renderSystem);

  int32_t GetVertexUniformSlot() { return mVertexUniformSlot; }

  int32_t GetFragmentUniformSlot() { return mFragmentUniformSlot; }

 private:
  void BuildVertexProgram();

  void BuildFragmentProgram();

 private:
  Geometry* mGeometry;
  Material* mMaterial;

  ShaderConfigration mConfigration = {};

  int32_t mVertexUniformSlot = -1;
  int32_t mFragmentUniformSlot = -1;

  slang::NodeHeap mHeap = {};

  std::vector<slang::Attribute*> mVertexInputState = {};
  std::vector<slang::Attribute*> mStageShareState = {};
  std::vector<slang::Node*> mStageShareStatement = {};

  slang::Attribute* mPosAttribute = {};

  slang::Program mVertexShader = {};
  slang::Program mFragmentShader = {};
};

}  // namespace cubic