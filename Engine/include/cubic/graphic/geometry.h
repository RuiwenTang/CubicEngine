#pragma once

#include <cubic/platform.h>
#include <cubic/render/types.h>

#include <memory>
#include <string>
#include <vector>

namespace cubic {

struct BufferAttribute {
  std::string name = {};
  VertexAttribute attribute = {};
};

struct CUBIC_API Geometry {
  std::vector<float> vertexData = {};
  std::vector<uint32_t> indexData = {};
  std::vector<BufferAttribute> attribute = {};

  Geometry(std::vector<float> vertex, std::vector<uint32_t> index, std::vector<BufferAttribute> attribute)
      : vertexData(std::move(vertex)), indexData(std::move(index)), attribute(std::move(attribute)) {}

  static std::shared_ptr<Geometry> CreatePlane(float width = 1.f, float height = 1.f, uint32_t widthSegment = 1,
                                               uint32_t heightSegment = 1);
};

}  // namespace cubic
