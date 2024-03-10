#pragma once

#include <cubic/platform.h>
#include <cubic/render/command_buffer.h>
#include <cubic/render/command_queue.h>
#include <cubic/render/render_pass.h>
#include <cubic/render/render_system.h>
#include <cubic/render/texture.h>

#include <memory>
#include <vector>

namespace cubic {

struct RenderContextInfo {
  // color attachment format
  TextureFormat colorFormat = TextureFormat::kInvalid;
  // depth attachment format
  TextureFormat depthFormat = TextureFormat::kInvalid;
  // whether has camera
  bool hasCamera = false;
};

class CUBIC_API RenderObject {
 public:
  virtual ~RenderObject() = default;

  virtual bool Prepare(RenderSystem* renderSystem, const RenderContextInfo& context) = 0;

  virtual void Draw(RenderPass* renderPass) = 0;
};

class CUBIC_API Renderer {
 public:
  Renderer(RenderSystem* renderSystem) : mRenderSystem(renderSystem) {}

  ~Renderer() = default;

  Renderer& SetClearColor(GPUColor clear);

  Renderer& SetNeedDepthBuffer(bool value);

  Renderer& SetAntialiasing(bool value);

  void Render(const std::shared_ptr<RenderObject>& renderObject);

  void Flush(const std::shared_ptr<Texture>& target);

 private:
  std::unique_ptr<RenderPass> BeginRenderPass(CommandBuffer* cmd, const std::shared_ptr<Texture>& target);

  void PrepareAttachments(const std::shared_ptr<Texture>& target);

  void DrawObjects(RenderPass* renderPass, TextureFormat format);

 private:
  RenderSystem* mRenderSystem;
  bool mNeedDepthBuffer = false;
  bool mAntialiasing = false;
  GPUColor mClearColor = {};

  std::shared_ptr<Texture> mMSAAColorTexture = {};
  std::shared_ptr<Texture> mDepthTexture = {};

  std::vector<std::shared_ptr<RenderObject>> mRenderObjects;
};

}  // namespace cubic
