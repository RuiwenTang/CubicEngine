#pragma once

#include <cubic/graphic/geometry.h>
#include <cubic/graphic/material.h>
#include <cubic/platform.h>

#include <memory>

namespace cubic {

class CUBIC_API Mesh {
 public:
  Mesh(std::shared_ptr<Geometry> geom, std::shared_ptr<Material> material);

  ~Mesh() = default;

  const std::shared_ptr<Geometry>& GetGeometry() const { return mGeometry; }

  const std::shared_ptr<Material>& GetMaterial() const { return mMaterial; }

 private:
  std::shared_ptr<Geometry> mGeometry;
  std::shared_ptr<Material> mMaterial;
};

}  // namespace cubic
