#include "render/vk/render_pipeline_vk.h"

#include <cubic/core/log.h>

#include <vector>

#include "render/vk/shader_module_vk.h"
#include "render/vk/vulkan_types.h"

namespace cubic {

RenderPipelineVK::RenderPipelineVK(VulkanDevice* device, VkPipeline pipeline, VkPipelineLayout layout,
                                   std::vector<VkDescriptorSetLayout> slotLayouts)
    : mDevice(device), mPipeline(pipeline), mLayout(layout), mSlotLayouts(std::move(slotLayouts)) {}

RenderPipelineVK::~RenderPipelineVK() {
  vkDestroyPipeline(mDevice->GetLogicalDevice(), mPipeline, nullptr);

  vkDestroyPipelineLayout(mDevice->GetLogicalDevice(), mLayout, nullptr);

  for (auto slot : mSlotLayouts) {
    vkDestroyDescriptorSetLayout(mDevice->GetLogicalDevice(), slot, nullptr);
  }
}

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

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  // multisample info
  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
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
  rasterizerState.cullMode = VK_CULL_MODE_NONE;
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

  // pipeline layout
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;             // Optional
  pipelineLayoutInfo.pSetLayouts = nullptr;          // Optional
  pipelineLayoutInfo.pushConstantRangeCount = 0;     // Optional
  pipelineLayoutInfo.pPushConstantRanges = nullptr;  // Optional

  VkPipelineLayout layout = VK_NULL_HANDLE;

  if (vkCreatePipelineLayout(device->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &layout) != VK_SUCCESS) {
    CUB_ERROR("[Vulkan backend] failed create pipeline layout !!");

    return {};
  }

  VkPipelineRenderingCreateInfo rendering_info{};
  rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
  rendering_info.colorAttachmentCount = desc->colorCount;
  std::vector<VkFormat> color_formats;
  for (uint32_t i = 0; i < desc->colorCount; i++) {
    color_formats.emplace_back(vk::TypeConvert(desc->pColorTargets[i].format));
  }

  rendering_info.pColorAttachmentFormats = color_formats.data();

  VkGraphicsPipelineCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  create_info.flags =
      VK_PIPELINE_CREATE_FAIL_ON_PIPELINE_COMPILE_REQUIRED_BIT | VK_PIPELINE_CREATE_EARLY_RETURN_ON_FAILURE_BIT;
  create_info.stageCount = static_cast<uint32_t>(shaderStages.size());
  create_info.pStages = shaderStages.data();
  create_info.pVertexInputState = &vertexInputState;
  create_info.pInputAssemblyState = &inputAssembly;
  create_info.pViewportState = &viewportState;
  create_info.pRasterizationState = &rasterizerState;
  create_info.pMultisampleState = &multisampling;
  // TODO setup depth stencil
  create_info.pDepthStencilState = nullptr;
  create_info.pColorBlendState = &blendState;
  create_info.pDynamicState = &dynamicState;
  create_info.layout = layout;

  create_info.pNext = &rendering_info;

  VkPipeline pipeline = VK_NULL_HANDLE;

  return std::shared_ptr<RenderPipelineVK>();
}

}  // namespace cubic
