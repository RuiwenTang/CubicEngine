#include <cubic/render/renderer.h>

namespace cubic {

Renderer& Renderer::SetClearColor(GPUColor clear) {
  mClearColor = std::move(clear);

  return *this;
}

Renderer& Renderer::SetNeedDepthBuffer(bool value) {
  mNeedDepthBuffer = value;

  return *this;
}

Renderer& Renderer::SetAntialiasing(bool value) {
  mAntialiasing = value;

  return *this;
}

void Renderer::Render(const std::shared_ptr<RenderObject>& renderObject) { mRenderObjects.emplace_back(renderObject); }

void Renderer::Flush(const std::shared_ptr<Texture>& target) {
  auto queue = mRenderSystem->GetCommandQueue(QueueType::kGraphic);

  auto cmd = queue->GenCommandBuffer();

  auto render_pass = BeginRenderPass(cmd.get(), target);

  DrawObjects(render_pass.get(), target->GetDescriptor().format);

  cmd->EndRenderPass(std::move(render_pass));

  queue->Submit(std::move(cmd));
}

std::unique_ptr<RenderPass> Renderer::BeginRenderPass(CommandBuffer* cmd, const std::shared_ptr<Texture>& target) {
  PrepareAttachments(target);

  ColorAttachment color_attachment{};
  color_attachment.clearValue = GPUColor{mClearColor.r, mClearColor.g, mClearColor.b, mClearColor.a};
  color_attachment.loadOp = LoadOp::kClear;

  if (mAntialiasing) {
    color_attachment.storeOp = StoreOp::kMSAAResolve;
    color_attachment.resolveTarget = target;
    color_attachment.target = mMSAAColorTexture;
  } else {
    color_attachment.storeOp = StoreOp::kStore;
    color_attachment.target = target;
  }

  DepthStencilAttachment ds_attachment{};

  if (mNeedDepthBuffer) {
    ds_attachment.depthClearValue = 1.f;
    ds_attachment.depthStoreOp = StoreOp::kDiscard;
    ds_attachment.depthLoadOp = LoadOp::kClear;
    ds_attachment.stencilClearValue = 0;
    ds_attachment.stencilLoadOp = LoadOp::kClear;
    ds_attachment.stencilStoreOp = StoreOp::kDiscard;

    ds_attachment.target = mDepthTexture;
  }

  RenderPassDescriptor desc{};
  desc.colorAttachmentCount = 1;
  desc.pColorAttachments = &color_attachment;

  if (mNeedDepthBuffer) {
    desc.pDepthStencilAttachment = &ds_attachment;
  }

  return cmd->BeginRenderPass(desc);
}

void Renderer::PrepareAttachments(const std::shared_ptr<Texture>& target) {
  if (mNeedDepthBuffer) {
    if (mDepthTexture == nullptr || mDepthTexture->GetDescriptor().width != target->GetDescriptor().width ||
        mDepthTexture->GetDescriptor().height != target->GetDescriptor().height) {
      TextureDescirptor desc{};

      desc.format = TextureFormat::kDepth24Stencil8;
      desc.width = target->GetDescriptor().width;
      desc.height = target->GetDescriptor().height;

      desc.usage = TextureUsage::kRenderTarget;
      desc.sample_count = mAntialiasing ? 4 : 1;

      mDepthTexture = mRenderSystem->CreateTexture(&desc);
    }
  }

  if (mAntialiasing) {
    if (mMSAAColorTexture == nullptr || mMSAAColorTexture->GetDescriptor().width != target->GetDescriptor().width ||
        mMSAAColorTexture->GetDescriptor().height != target->GetDescriptor().height) {
      TextureDescirptor desc = target->GetDescriptor();

      desc.sample_count = 4;
      desc.usage = TextureUsage::kRenderTarget;

      mMSAAColorTexture = mRenderSystem->CreateTexture(&desc);
    }
  }
}

void Renderer::DrawObjects(RenderPass* renderPass, TextureFormat format) {
  std::vector<RenderObject*> drawList{};

  for (const auto& object : mRenderObjects) {
    if (object->Prepare(mRenderSystem, format)) {
      drawList.emplace_back(object.get());
    }
  }

  for (const auto& object : drawList) {
    object->Draw(renderPass);
  }
}

}  // namespace cubic
