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

  return 0;
}