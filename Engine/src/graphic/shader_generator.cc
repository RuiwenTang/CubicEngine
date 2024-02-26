#include "graphic/shader_generator.h"

#include "slang/attribute.h"
#include "slang/function.h"
#include "slang/operation.h"
#include "slang/statement.h"

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

      mVertexInputState.emplace_back(in_attr);
      mStageShareState.emplace_back(out_attr);

      auto statement = mHeap.Allocate<slang::Statement>();

      statement->AddExpression(mHeap.Allocate<slang::BinaryOperation>(slang::Operator::kEqual, out_attr, in_attr));

      mStageShareStatement.emplace_back(statement);

      if (it->name == "position") {
        mPosAttribute = in_attr;
      }
    }
  }

  if (mPosAttribute == nullptr) {
    mPosAttribute = mHeap.Allocate<slang::Attribute>(pos_it->attribute.location, pos_it->attribute.format,
                                                     "in_" + pos_it->name, true);
    mVertexInputState.emplace_back(mPosAttribute);
  }

  BuildVertexProgram();

  return true;
}

void ShaderGenerator::BuildVertexProgram() {
  for (const auto& i : mVertexInputState) {
    mVertexShader.AddToGlobalScope(i);
  }

  for (const auto& o : mStageShareState) {
    mVertexShader.AddToGlobalScope(o);
  }

  auto vs_main = mHeap.Allocate<slang::StatementFunction>("main");

  for (auto statemen : mStageShareStatement) {
    vs_main->AddStatement(statemen);
  }

  auto pos_out = mHeap.Allocate<slang::Statement>();

  {
    auto pos_assign = mHeap.Allocate<slang::BinaryOperation>(
        slang::Operator::kEqual, mHeap.Allocate<slang::NamedNode>("gl_Position"), mPosAttribute);

    pos_out->AddExpression(pos_assign);
  }

  vs_main->AddStatement(pos_out);

  mVertexShader.SetMainFunc(vs_main);
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
