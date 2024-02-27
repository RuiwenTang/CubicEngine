#include "../src/graphic/shader_generator.h"

#include <cubic/cubic.h>

int main(int argc, const char** argv) {
  auto geom = cubic::Geometry::CreatePlane();

  auto material = std::make_shared<cubic::ColorMaterial>(1.f, 0.f, 0.f, 1.f);

  cubic::ShaderGenerator sg{geom.get(), material.get()};

  if (!sg.Prepare()) {
    CUB_ERROR("shader generator prepare failed");

    return -1;
  }

  CUB_INFO("vertex shader: \n");
  CUB_INFO("\n {}", sg.GenVertexShader());

  CUB_INFO("fragment shader: \n");
  CUB_INFO("\n {}", sg.GenFragmentShader());

  cubic::Application::Config config;
  config.name = "Sandbox Application";

  if (!cubic::Application::Init(config)) {
    CUB_ERROR("Failed init Application!!");

    cubic::Application::Terminate();

    return -1;
  }

  auto app = cubic::Application::Get();

  cubic::ShaderModuleDescriptor vs_desc{};
  vs_desc.stage = cubic::ShaderStage::kVertexShader;
  vs_desc.code = sg.GenVertexShader();
  vs_desc.label = "gened vertex shader";

  auto vs_shader = app->GetRenderSystem()->CreateShaderModule(&vs_desc);

  cubic::ShaderModuleDescriptor fs_desc{};
  fs_desc.stage = cubic::kFragmentShader;
  fs_desc.code = sg.GenFragmentShader();
  fs_desc.label = "gened fragment shader";

  auto fs_shader = app->GetRenderSystem()->CreateShaderModule(&fs_desc);

  return 0;
}