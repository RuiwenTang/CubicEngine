#include <cubic/graphic/mesh.h>

#include "graphic/shader_generator.h"
#include "graphic/stage_buffer.h"

namespace cubic {

std::shared_ptr<ShaderModule> gen_shader(ShaderStage stage, const std::string& code, RenderSystem* renderSystem) {
  ShaderModuleDescriptor desc{};

  desc.stage = stage;
  desc.code = code;
  desc.label = "mesh shader";

  return renderSystem->CreateShaderModule(&desc);
}

Mesh::Mesh(std::shared_ptr<Geometry> geom, std::shared_ptr<Material> material)
    : mGeometry(std::move(geom)), mMaterial(std::move(material)) {}

bool Mesh::Prepare(RenderSystem* renderSystem, TextureFormat targetFormat) {
  if (!PreparePipeline(renderSystem, targetFormat)) {
    return false;
  }

  if (!PrepareBuffer(renderSystem)) {
    return false;
  }

  if (!PrepareBindings(renderSystem)) {
    return false;
  }

  return true;
}

void Mesh::Draw(RenderPass* renderPass) {
  // bind pipeline
  renderPass->BindPipeline(mRenderPipeline);
  // setup vertex data
  renderPass->SetVertexBuffer(mVertexData.buffer, 0, mVertexData.offset);
  // setup index data
  renderPass->SetIndexBuffer(mIndexData.buffer, mIndexData.offset);

  // setup binding group
  renderPass->SetBindGroup(0, mMaterialBinding);

  renderPass->DrawElements(mGeometry->indexData.size(), 0);
}

bool Mesh::PreparePipeline(RenderSystem* renderSystem, TextureFormat targetFormat) {
  if (mRenderPipeline) {
    return true;
  }

  ShaderGenerator sg{mGeometry.get(), mMaterial.get()};

  if (!sg.Prepare()) {
    return false;
  }

  auto vs = gen_shader(ShaderStage::kVertexShader, sg.GenVertexShader(), renderSystem);
  auto fs = gen_shader(ShaderStage::kFragmentShader, sg.GenFragmentShader(), renderSystem);

  if (!vs || !fs) {
    return false;
  }

  RenderPipelineDescriptor desc{};

  desc.vertexShader = vs;
  desc.fragmentShader = fs;
  desc.vertexBuffer = sg.GenVertexBufferLayout();
  desc.layout = sg.GenPipelineLayout(renderSystem);

  // TODO pass sample count to mesh
  desc.sampleCount = 4;

  ColorTargetState colorTarget{};
  colorTarget.format = targetFormat;

  desc.colorCount = 1;
  desc.pColorTargets = &colorTarget;

  mRenderPipeline = renderSystem->CreateRenderPipeline(&desc);

  return mRenderPipeline != nullptr;
}

bool Mesh::PrepareBuffer(RenderSystem* renderSystem) {
  if (mRenderData == nullptr) {
    if (!mMeshUniforms.empty()) {
      mMeshUniforms.clear();
    }

    if (!mMaterialUniforms.empty()) {
      mMaterialUniforms.clear();
    }

    AllocBuffer(renderSystem);

  } else {
  }

  return true;
}

void Mesh::AllocBuffer(RenderSystem* renderSystem) {
  StageBuffer stageBuffer(renderSystem->GetMinBufferAlignment());

  // allocate memory for vertex data
  {
    auto range = stageBuffer.Allocate(mGeometry->vertexData.size() * sizeof(float));

    mVertexData.offset = range.offset;
    mVertexData.length = range.length;
  }
  // allocate memory for index data
  {
    auto range = stageBuffer.Allocate(mGeometry->indexData.size() * sizeof(uint32_t));

    mIndexData.offset = range.offset;
    mIndexData.length = range.length;
  }

  // TODO allocate uniforms in vertex stage
  auto materialData = mMaterial->GetResourceData();

  for (size_t i = 0; i < materialData.size(); i++) {
    auto range = stageBuffer.Allocate(materialData[i].length, true);

    mMaterialUniforms.emplace_back(BufferView{
        nullptr,
        range.offset,
        range.length,
    });
  }

  std::vector<uint8_t> raw_data(stageBuffer.GetCurrentSize());

  // copy vertex data
  std::memcpy(raw_data.data() + mVertexData.offset, mGeometry->vertexData.data(), mVertexData.length);
  // copy index data
  std::memcpy(raw_data.data() + mIndexData.offset, mGeometry->indexData.data(), mIndexData.length);

  // TODO copy uniforms in vertex stage

  // copy uniforms in fragment stage
  for (size_t i = 0; i < mMaterialUniforms.size(); i++) {
    std::memcpy(raw_data.data() + mMaterialUniforms[i].offset, materialData[i].data, materialData[i].length);
  }

  // allocate GPUBuffer read only buffer
  {
    BufferDescriptor desc{};
    desc.storageMode = BufferStorageMode::kGPUOnly;
    desc.size = raw_data.size();
    desc.usage =
        BufferUsage::kBuffCopyDst | BufferUsage::kBuffUniform | BufferUsage::kBuffIndex | BufferUsage::kBuffVertex;

    mRenderData = renderSystem->CreateBuffer(&desc);
  }

  // allocate copy src buffer
  BufferDescriptor desc{};
  desc.storageMode = BufferStorageMode::kCPUOnly;
  desc.size = raw_data.size();
  desc.usage = BufferUsage::kBuffCopySrc;
  desc.data = raw_data.data();

  auto buffer = renderSystem->CreateBuffer(&desc);

  // copy buffer to RenderData
  auto queue = renderSystem->GetCommandQueue(QueueType::kGraphic);

  auto cmd = queue->GenCommandBuffer();

  cmd->CopyBufferToBuffer(mRenderData, 0, buffer, 0, raw_data.size());

  queue->Submit(std::move(cmd));

  mVertexData.buffer = mRenderData;
  mIndexData.buffer = mRenderData;

  for (auto& view : mMaterialUniforms) {
    view.buffer = mRenderData;
  }
}

bool Mesh::PrepareBindings(RenderSystem* renderSystem) {
  // TODO support mark dirty

  if (mMaterialBinding != nullptr) {
    return true;
  }

  {
    auto layout = mRenderPipeline->GetLayout()->GetGroup(0);

    std::vector<GroupEntry> entries{};

    for (size_t i = 0; i < mMaterialUniforms.size(); i++) {
      entries.emplace_back(GroupEntry{
          static_cast<uint32_t>(i),
          EntryType::kUniformBuffer,
          BindResource{
              mMaterialUniforms[i],
          },
      });
    }

    mMaterialBinding = renderSystem->CreateBindGroup(layout, std::move(entries));
  }

  return true;
}

}  // namespace cubic
