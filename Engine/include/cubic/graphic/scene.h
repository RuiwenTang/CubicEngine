#pragma once

#include <cubic/platform.h>
#include <cubic/render/renderer.h>

#include <memory>
#include <vector>

namespace cubic {

class CUBIC_API Scene : public RenderObject {
 public:
  Scene() = default;

  ~Scene() override = default;

  bool Prepare(RenderSystem* renderSystem, const RenderContextInfo& context) override;

  void Draw(RenderPass* renderPass) override;

  void AddRenderObject(std::shared_ptr<RenderObject> renderObject);

 private:
  std::vector<std::shared_ptr<RenderObject>> mRenderObjects = {};
};

}  // namespace cubic
