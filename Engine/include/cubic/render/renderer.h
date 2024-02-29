#pragma once

#include <cubic/platform.h>
#include <cubic/render/command_buffer.h>
#include <cubic/render/command_queue.h>
#include <cubic/render/render_pass.h>
#include <cubic/render/render_system.h>
#include <cubic/render/texture.h>

namespace cubic {

class CUBIC_API Renderer {
 public:
  Renderer(RenderSystem* renderSystem) : mRenderSystem(renderSystem) {}

  ~Renderer() = default;

  Renderer& SetClearColor(GPUColor clear);

  Renderer& SetNeedDepthBuffer(bool value);

  Renderer& SetAntialiasing(bool value);

  void Flush(const std::shared_ptr<Texture>& target);

 private:
  std::unique_ptr<RenderPass> BeginRenderPass(CommandBuffer* cmd, const std::shared_ptr<Texture>& target);

  void PrepareAttachments(const std::shared_ptr<Texture>& target);

 private:
  RenderSystem* mRenderSystem;
  bool mNeedDepthBuffer = false;
  bool mAntialiasing = false;
  GPUColor mClearColor = {};

  std::shared_ptr<Texture> mMSAAColorTexture = {};
  std::shared_ptr<Texture> mDepthTexture = {};
};

}  // namespace cubic
