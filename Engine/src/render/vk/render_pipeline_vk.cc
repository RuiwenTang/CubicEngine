#include "render/vk/render_pipeline_vk.h"

#include <cubic/core/log.h>

#include <vector>

#include "render/vk/pipeline_layout_vk.h"
#include "render/vk/shader_module_vk.h"
#include "render/vk/vulkan_types.h"

namespace cubic {

RenderPipelineVK::RenderPipelineVK(VulkanDevice* device, VkPipeline pipeline)
    : RenderPipeline(), mDevice(device), mPipeline(pipeline) {}

RenderPipelineVK::~RenderPipelineVK() { vkDestroyPipeline(mDevice->GetLogicalDevice(), mPipeline, nullptr); }

std::shared_ptr<RenderPipelineVK> RenderPipelineVK::Create(VulkanDevice* device, RenderPipelineDescriptor* desc) {
  // shaders
  std::vector<VkPipelineShaderStageCreateInfo> shaderStages(2);
  shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
  shaderStages[0].module = dynamic_cast<ShaderModuleVK*>(desc->vertexShader.get())->GetNativeShader();
  shaderStages[0].pName = "main";

  shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  shaderStages[1].module = dynamic_cast<ShaderModuleVK*>(desc->fragmentShader.get())->GetNativeShader();
  shaderStages[1].pName = "main";

  // vertex input
  // TODO reflect this info from shader module
  VkPipelineVertexInputStateCreateInfo vertexInputState{};
  vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  std::vector<VkVertexInputBindingDescription> vertex_bindings{};
  std::vector<VkVertexInputAttributeDescription> vertex_attrs{};

  for (size_t i = 0; i < desc->vertexBuffer.size(); i++) {
    vertex_bindings.emplace_back(VkVertexInputBindingDescription{
        static_cast<uint32_t>(i),
        static_cast<uint32_t>(desc->vertexBuffer[i].stride),
        vk::TypeConvert(desc->vertexBuffer[i].stepMode),
    });

    for (auto const& attr : desc->vertexBuffer[i].attribute) {
      vertex_attrs.emplace_back(VkVertexInputAttributeDescription{
          attr.location,
          static_cast<uint32_t>(i),
          vk::TypeConvertForInput(attr.format),
          static_cast<uint32_t>(attr.offset),
      });
    }
  }

  vertexInputState.vertexBindingDescriptionCount = static_cast<uint32_t>(vertex_bindings.size());
  vertexInputState.pVertexBindingDescriptions = vertex_bindings.data();
  vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attrs.size());
  vertexInputState.pVertexAttributeDescriptions = vertex_attrs.data();

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // multisample info
  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = vk::TypeConvert(desc->sampleCount);
  multisampling.minSampleShading = 1.0f;           // Optional
  multisampling.pSampleMask = nullptr;             // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE;  // Optional
  multisampling.alphaToOneEnable = VK_FALSE;       // Optional

  // dynamic state
  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();

  // viewport and scissor
  std::vector<VkViewport> viewports(desc->colorCount);
  std::vector<VkRect2D> scissors(desc->colorCount);
  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = desc->colorCount;
  viewportState.scissorCount = desc->colorCount;
  viewportState.pViewports = viewports.data();
  viewportState.pScissors = scissors.data();

  // rasterizer info
  VkPipelineRasterizationStateCreateInfo rasterizerState{};
  rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizerState.depthClampEnable = VK_FALSE;
  rasterizerState.rasterizerDiscardEnable = VK_FALSE;
  rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizerState.depthBiasEnable = VK_FALSE;
  rasterizerState.depthBiasConstantFactor = 0.0f;  // Optional
  rasterizerState.depthBiasClamp = 0.0f;           // Optional
  rasterizerState.depthBiasSlopeFactor = 0.0f;     // Optional
  // TODO pass face and cull mode through pipeline descriptor
  rasterizerState.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE;

  std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};
  for (uint32_t i = 0; i < desc->colorCount; i++) {
    VkPipelineColorBlendAttachmentState attachment{};
    attachment.colorWriteMask = vk::TypeConvert(desc->pColorTargets[i].writeMask);
    if (desc->pColorTargets[i].blend) {
      attachment.blendEnable = VK_TRUE;
      attachment.srcColorBlendFactor = vk::TypeConvert(desc->pColorTargets[i].blend->color.srcFactor);
      attachment.dstColorBlendFactor = vk::TypeConvert(desc->pColorTargets[i].blend->color.dstFactor);
      attachment.colorBlendOp = vk::TypeConvert(desc->pColorTargets[i].blend->color.operation);
      attachment.srcAlphaBlendFactor = vk::TypeConvert(desc->pColorTargets[i].blend->alpha.srcFactor);
      attachment.dstAlphaBlendFactor = vk::TypeConvert(desc->pColorTargets[i].blend->alpha.dstFactor);
      attachment.alphaBlendOp = vk::TypeConvert(desc->pColorTargets[i].blend->alpha.operation);
    } else {
      attachment.blendEnable = VK_FALSE;
      attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
      attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
      attachment.colorBlendOp = VK_BLEND_OP_ADD;
      attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
      attachment.alphaBlendOp = VK_BLEND_OP_ADD;
    }

    colorBlendAttachments.emplace_back(attachment);
  }

  VkPipelineColorBlendStateCreateInfo blendState{};
  blendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  blendState.logicOpEnable = VK_FALSE;
  blendState.logicOp = VK_LOGIC_OP_COPY;
  blendState.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
  blendState.pAttachments = colorBlendAttachments.data();

  VkPipelineRenderingCreateInfo rendering_info{};
  rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
  rendering_info.colorAttachmentCount = desc->colorCount;
  std::vector<VkFormat> color_formats;
  for (uint32_t i = 0; i < desc->colorCount; i++) {
    color_formats.emplace_back(vk::TypeConvert(desc->pColorTargets[i].format));
  }

  rendering_info.pColorAttachmentFormats = color_formats.data();

  VkPipelineDepthStencilStateCreateInfo depthStencilState{};
  depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  if (desc->depthStencil) {
    rendering_info.depthAttachmentFormat = vk::TypeConvert(desc->depthStencil->format);
    depthStencilState.depthTestEnable = VK_TRUE;
    depthStencilState.depthWriteEnable = VK_TRUE;
    depthStencilState.depthCompareOp = vk::TypeConvert(desc->depthStencil->depthCompare);
  }

  VkGraphicsPipelineCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  create_info.flags = 0;
  create_info.stageCount = static_cast<uint32_t>(shaderStages.size());
  create_info.pStages = shaderStages.data();
  create_info.pVertexInputState = &vertexInputState;
  create_info.pInputAssemblyState = &inputAssembly;
  create_info.pViewportState = &viewportState;
  create_info.pRasterizationState = &rasterizerState;
  create_info.pMultisampleState = &multisampling;
  // TODO setup depth stencil
  if (desc->depthStencil) {
    create_info.pDepthStencilState = &depthStencilState;
  } else {
    create_info.pDepthStencilState = nullptr;
  }
  create_info.pColorBlendState = &blendState;
  create_info.pDynamicState = &dynamicState;
  create_info.layout = VK_NULL_HANDLE;

  create_info.pNext = &rendering_info;

  VkPipeline pipeline = VK_NULL_HANDLE;

  if (auto ret = vkCreateGraphicsPipelines(device->GetLogicalDevice(), nullptr, 1, &create_info, nullptr, &pipeline) !=
                 VK_SUCCESS) {
    CUB_ERROR("[Vulkan backend] Failed create render pipeline !!");
    return {};
  }

  return std::make_shared<RenderPipelineVK>(device, pipeline);
}

void RenderPipelineVK::Bind(VkCommandBuffer cmd) { vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline); }

}  // namespace cubic
