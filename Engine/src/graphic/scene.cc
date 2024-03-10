#include <cubic/graphic/scene.h>

namespace cubic {

bool Scene::Prepare(RenderSystem* renderSystem, const RenderContextInfo& context) {
  for (auto& object : mRenderObjects) {
    if (!object->Prepare(renderSystem, context)) {
      return false;
    }
  }

  return true;
}

void Scene::Draw(RenderPass* renderPass) {
  for (auto& object : mRenderObjects) {
    object->Draw(renderPass);
  }
}

void Scene::AddRenderObject(std::shared_ptr<RenderObject> renderObject) {
  mRenderObjects.emplace_back(std::move(renderObject));
}

}  // namespace cubic
