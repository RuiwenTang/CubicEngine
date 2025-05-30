#pragma once

#include <cubic/graphic/geometry.h>
#include <cubic/graphic/material.h>
#include <cubic/graphic/transform.h>
#include <cubic/platform.h>
#include <cubic/render/buffer.h>
#include <cubic/render/render_pipeline.h>
#include <cubic/render/renderer.h>

#include <memory>
#include <vector>

namespace cubic {

class CUBIC_API Mesh : public RenderObject {
 public:
  Mesh(std::shared_ptr<Geometry> geom, std::shared_ptr<Material> material);

  ~Mesh() override = default;

  const std::shared_ptr<Geometry>& GetGeometry() const { return mGeometry; }

  const std::shared_ptr<Material>& GetMaterial() const { return mMaterial; }

  bool Prepare(RenderSystem* renderSystem, const RenderContextInfo& context) override;

  void Draw(RenderPass* renderPass) override;

  Transform& GetTransform() { return mTransform; }

 private:
  bool PreparePipeline(RenderSystem* renderSystem, const RenderContextInfo& context);

  bool PrepareBuffer(RenderSystem* renderSystem);

  bool PrepareBindings(RenderSystem* renderSystem);

  void AllocBuffer(RenderSystem* renderSystem);

  void UpdateBuffer(RenderSystem* renderSystem);

 private:
  Transform mTransform = {};

  std::shared_ptr<Geometry> mGeometry;
  std::shared_ptr<Material> mMaterial;

  std::shared_ptr<RenderPipeline> mRenderPipeline;
  std::shared_ptr<Buffer> mRenderData;

  BufferView mVertexData = {};
  BufferView mIndexData = {};

  std::vector<BufferView> mMeshUniforms = {};
  std::vector<BufferView> mMaterialUniforms = {};
};

}  // namespace cubic
