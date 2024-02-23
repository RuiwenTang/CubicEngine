#pragma once

#include <cubic/graphic/geometry.h>
#include <cubic/graphic/material.h>

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
  Geometry* mGeometry;
  Material* mMaterial;

  slang::NodeHeap mHeap = {};

  slang::Program mVertexShader = {};
  slang::Program mFragmentShader = {};
};

}  // namespace cubic