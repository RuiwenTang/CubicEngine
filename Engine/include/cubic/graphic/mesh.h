#pragma once

#include <cubic/graphic/geometry.h>
#include <cubic/graphic/material.h>
#include <cubic/platform.h>
#include <cubic/render/render_pipeline.h>
#include <cubic/render/renderer.h>

#include <memory>

namespace cubic {

class CUBIC_API Mesh : public RenderObject {
 public:
  Mesh(std::shared_ptr<Geometry> geom, std::shared_ptr<Material> material);

  ~Mesh() = default;

  const std::shared_ptr<Geometry>& GetGeometry() const { return mGeometry; }

  const std::shared_ptr<Material>& GetMaterial() const { return mMaterial; }

  bool Prepare(RenderSystem* renderSystem, TextureFormat targetFormat) override;

  void Draw(RenderPass* renderPass) override;

 private:
  bool PreparePipeline(RenderSystem* renderSystem, TextureFormat targetFormat);

 private:
  std::shared_ptr<Geometry> mGeometry;
  std::shared_ptr<Material> mMaterial;

  std::shared_ptr<RenderPipeline> mRenderPipeline;
};

}  // namespace cubic
