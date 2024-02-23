#include "graphic/shader_generator.h"

#include "slang/attribute.h"
#include "slang/function.h"

namespace cubic {

bool ShaderGenerator::Prepare() {
  if (mGeometry == nullptr || mMaterial == nullptr) {
    return false;
  }

  auto geom_inputs = mGeometry->attribute;
  auto pos_it = std::find_if(geom_inputs.begin(), geom_inputs.end(),
                             [](BufferAttribute& attr) { return attr.name == "position"; });

  if (pos_it == geom_inputs.end()) {
    return false;
  }

  auto frag_inputs = mMaterial->GetInput();

  slang::Attribute* pos_attr = nullptr;

  uint32_t out_index = 0;
  // find out what attribute is needed in the final shader
  for (const auto& mi : frag_inputs) {
    auto it = std::find_if(geom_inputs.begin(), geom_inputs.end(), [&mi](BufferAttribute& attr) {
      return attr.name == mi.name && attr.attribute.format == mi.format;
    });

    if (it != geom_inputs.end()) {
      auto in_attr =
          mHeap.Allocate<slang::Attribute>(it->attribute.location, it->attribute.format, "in_" + it->name, true);

      auto out_attr = mHeap.Allocate<slang::Attribute>(out_index, mi.format, mi.name, false);

      mVertexShader.AddInput(in_attr);
      mVertexShader.AddOutput(out_attr);

      mFragmentShader.AddInput(mHeap.Allocate<slang::Attribute>(out_index, mi.format, mi.name, true));

      if (it->name == "position") {
        pos_attr = in_attr;
      }
    }
  }

  if (pos_attr == nullptr) {
    pos_attr = mHeap.Allocate<slang::Attribute>(pos_it->attribute.location, pos_it->attribute.format,
                                                "in_" + pos_it->name, true);

    mVertexShader.AddInput(pos_attr);
  }

  auto vs_main = mHeap.Allocate<slang::StatementFunction>("main");

  mVertexShader.SetMainFunc(vs_main);

  return true;
}

std::string ShaderGenerator::GenVertexShader() {
  std::string source = {};

  mVertexShader.WriteTo(source);

  return source;
}

std::string ShaderGenerator::GenFragmentShader() {
  std::string source = {};

  mFragmentShader.WriteTo(source);

  return source;
}

}  // namespace cubic
