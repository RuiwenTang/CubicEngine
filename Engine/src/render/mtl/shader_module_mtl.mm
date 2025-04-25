#include "render/mtl/shader_module_mtl.h"

#include <cubic/core/log.h>
#include <cubic/platform.h>
#include <algorithm>
#include <spirv_msl.hpp>
#include <spirv_parser.hpp>
#include <spirv_reflect.hpp>
#include <vector>

namespace cubic {

bool StageGroup::AddEntry(StageEntryInfo entry) {
  auto it =
      std::find_if(entries.begin(), entries.end(), [&entry](StageEntryInfo& e) { return e.binding == entry.binding; });

  if (it == entries.end()) {
    entries.push_back(entry);
    return true;
  }

  return false;
}

BindGroupLayout StageGroup::GenLayout(ShaderStage stage) {
  BindGroupLayout layout{set};

  for (auto& entry : entries) {
    layout.AddBinding(entry.binding, entry.type, stage);
  }

  return layout;
}

struct StagePipelineInfo {
  std::vector<StageGroup> groups;

  StageGroup& GetGroup(uint32_t set) {
    auto it = std::find_if(groups.begin(), groups.end(), [set](StageGroup& g) { return g.set == set; });

    if (it != groups.end()) {
      return *it;
    }

    StageGroup group{};
    group.set = set;

    groups.emplace_back(group);

    return groups.back();
  }

  std::vector<BindGroupLayout> GenLayouts(ShaderStage stage) {
    std::vector<BindGroupLayout> layouts;

    for (auto& group : groups) {
      layouts.push_back(group.GenLayout(stage));
    }

    return layouts;
  }
};

static void setup_compiler(spirv_cross::CompilerMSL& compiler) {
  auto option = compiler.get_msl_options();

#ifdef CUBIC_PLATFORM_MACOS
  option.platform = spirv_cross::CompilerMSL::Options::macOS;
#else
  option.platform = spirv_cross::CompilerMSL::Options::iOS;
#endif

  option.msl_version = spirv_cross::CompilerMSL::Options::make_msl_version(2, 0);
  // use argument buffer to support ** BindGroup **
  option.argument_buffers = false;

  compiler.set_msl_options(option);
}

static StagePipelineInfo ensure_bind_group(spirv_cross::CompilerMSL& compiler) {
  auto uniform_resources = compiler.get_shader_resources();

  auto execution_model = compiler.get_execution_model();

  StagePipelineInfo pipeline_info{};

  // skip buffer 0 for vertex shader
  uint32_t buffer_index = execution_model == spv::ExecutionModelVertex ? 1 : 0;
  uint32_t texture_index = 0;
  uint32_t sampler_index = 0;

  for (const auto& resource : uniform_resources.uniform_buffers) {
    auto set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
    auto binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

    auto& group = pipeline_info.GetGroup(set);

    StageEntryInfo entry{};
    entry.type = EntryType::kUniformBuffer;
    entry.binding = binding;
    entry.slot = buffer_index++;

    group.AddEntry(entry);

    spirv_cross::MSLResourceBinding binding_info{};
    binding_info.basetype = spirv_cross::SPIRType::BaseType::Void;
    binding_info.stage = execution_model;
    binding_info.desc_set = set;
    binding_info.binding = binding;
    binding_info.count = 1;
    binding_info.msl_buffer = entry.slot;

    compiler.add_msl_resource_binding(binding_info);
  }

  for (const auto& resource : uniform_resources.separate_images) {
    auto set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
    auto binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

    auto& group = pipeline_info.GetGroup(set);

    StageEntryInfo entry{};
    entry.type = EntryType::kTexture;
    entry.binding = binding;
    group.AddEntry(entry);

    spirv_cross::MSLResourceBinding binding_info{};
    binding_info.basetype = spirv_cross::SPIRType::BaseType::SampledImage;
    binding_info.stage = execution_model;
    binding_info.desc_set = set;
    binding_info.binding = binding;
    binding_info.count = 1;
    binding_info.msl_texture = texture_index++;

    compiler.add_msl_resource_binding(binding_info);
  }

  for (const auto& resource : uniform_resources.separate_samplers) {
    auto set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
    auto binding = compiler.get_decoration(resource.id, spv::DecorationBinding);

    auto& group = pipeline_info.GetGroup(set);
    StageEntryInfo entry{};

    entry.type = EntryType::kSampler;
    entry.binding = binding;
    group.AddEntry(entry);

    spirv_cross::MSLResourceBinding binding_info{};
    binding_info.basetype = spirv_cross::SPIRType::BaseType::Sampler;
    binding_info.stage = execution_model;
    binding_info.desc_set = set;
    binding_info.binding = binding;
    binding_info.count = 1;
    binding_info.msl_sampler = sampler_index++;
    compiler.add_msl_resource_binding(binding_info);
  }

  return pipeline_info;
}

ShaderModuleMTL::ShaderModuleMTL(ShaderStage stage, std::string label, std::vector<BindGroupLayout> groups,
                                 std::vector<StageGroup> stageGroups, id<MTLLibrary> shader)
    : ShaderModule(stage, std::move(label), std::move(groups)), mGroups(std::move(stageGroups)), mNativeShader(shader) {
  NSString* name = @"main0";

  mEntryPoint = [mNativeShader newFunctionWithName:name];

  [name release];
}

ShaderModuleMTL::~ShaderModuleMTL() {
  [mEntryPoint release];
  [mNativeShader release];
}

id<MTLLibrary> ShaderModuleMTL::GetNativeShader() const { return mNativeShader; }

id<MTLFunction> ShaderModuleMTL::GetEntryPoint() const { return mEntryPoint; }

std::shared_ptr<ShaderModule> ShaderModuleMTL::Compile(id<MTLDevice> gpu, ShaderModuleDescriptor* desc,
                                                       const std::vector<uint32_t>& spv) {
  spirv_cross::Parser parser(spv);

  parser.parse();

  spirv_cross::CompilerMSL compiler(parser.get_parsed_ir());

  auto info = ensure_bind_group(compiler);

  setup_compiler(compiler);

  auto result = compiler.compile();

  CUB_DEBUG("Metal backend shader compiler:\n result shader for : [ {} ] is : \n {} ", desc->label, result);

  // compile MSl shader
  MTLCompileOptions* option = [MTLCompileOptions new];
  option.languageVersion = MTLLanguageVersion2_0;

  NSError* err = nil;
  NSString* msl_source = [NSString stringWithCString:result.c_str() encoding:NSUTF8StringEncoding];

  id<MTLLibrary> lib = [gpu newLibraryWithSource:msl_source options:option error:&err];

  [msl_source release];

  if (err != nil) {
    const char* msg = [[err localizedDescription] UTF8String];

    CUB_ERROR("[Metal backend] compile shader : [ {} ] meet error: {}", desc->label, msg);

    return {};
  }

  auto shader_module =
      std::make_shared<ShaderModuleMTL>(desc->stage, desc->label, info.GenLayouts(desc->stage), info.groups, lib);

  if (shader_module->GetEntryPoint() == nil) {
    CUB_ERROR("[Metal backend] can not found entry point in shader: [ {} ]", desc->label);
    return {};
  }

  return shader_module;
}

}  // namespace cubic
