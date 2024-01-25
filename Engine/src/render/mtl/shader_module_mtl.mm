#include "render/mtl/shader_module_mtl.h"

#include <cubic/core/log.h>
#include <cubic/platform.h>
#include <spirv_msl.hpp>
#include <spirv_parser.hpp>

namespace cubic {

static void setup_compiler(spirv_cross::CompilerMSL& compiler) {
  auto option = compiler.get_msl_options();

#ifdef CUBIC_PLATFORM_MACOS
  option.platform = spirv_cross::CompilerMSL::Options::macOS;
#else
  option.platform = spirv_cross::CompilerMSL::Options::iOS;
#endif

  option.msl_version = spirv_cross::CompilerMSL::Options::make_msl_version(2, 0);

  compiler.set_msl_options(option);
}

ShaderModuleMTL::ShaderModuleMTL(ShaderStage stage, std::string label, id<MTLLibrary> shader)
    : ShaderModule(stage, std::move(label)), mNativeShader(shader) {
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

  auto shader_module = std::make_shared<ShaderModuleMTL>(desc->stage, desc->label, lib);

  if (shader_module->GetEntryPoint() == nil) {
    CUB_ERROR("[Metal backend] can not found entry point in shader: [ {} ]", desc->label);
    return {};
  }

  return shader_module;
}

}
