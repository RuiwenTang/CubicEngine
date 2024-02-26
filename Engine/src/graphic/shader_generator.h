#pragma once

#include <cubic/graphic/geometry.h>
#include <cubic/graphic/material.h>

#include <vector>

#include "slang/program.h"

namespace cubic {

class ShaderGenerator {
 public:
  ShaderGenerator(Geometry* geometry, Material* material) : mGeometry(geometry), mMaterial(material) {}

  ~ShaderGenerator() = default;

  bool Prepare();

  std::string GenVertexShader();

  std::string GenFragmentShader();

 private:
  void BuildVertexProgram();

 private:
  Geometry* mGeometry;
  Material* mMaterial;

  slang::NodeHeap mHeap = {};

  std::vector<slang::Node*> mVertexInputState = {};
  std::vector<slang::Node*> mStageShareState = {};
  std::vector<slang::Node*> mStageShareStatement = {};

  slang::Node* mPosAttribute = {};

  slang::Program mVertexShader = {};
  slang::Program mFragmentShader = {};
};

}  // namespace cubic