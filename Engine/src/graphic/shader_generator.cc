#include "graphic/shader_generator.h"

#include <cubic/render/render_system.h>

#include "slang/attribute.h"
#include "slang/function.h"
#include "slang/operation.h"
#include "slang/statement.h"
#include "slang/types.h"
#include "slang/uniform.h"

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
      auto in_attr = mHeap.Allocate<slang::Attribute>(
          it->attribute.location, slang::ConvertScalarType(it->attribute.format), "in_" + it->name, true);

      auto out_attr = mHeap.Allocate<slang::Attribute>(out_index, slang::ConvertScalarType(mi.format), mi.name, false);

      mVertexInputState.emplace_back(in_attr);
      mStageShareState.emplace_back(out_attr);

      auto statement = mHeap.Allocate<slang::Statement>();

      statement->AddExpression(mHeap.Allocate<slang::BinaryOperation>(
          slang::Operator::kEqual, out_attr,
          mHeap.Allocate<slang::TypeConvertOperation>(out_attr->GetScalarType(), in_attr->GetScalarType(), in_attr)));

      mStageShareStatement.emplace_back(statement);

      if (it->name == "position") {
        mPosAttribute = in_attr;
      }
    }
  }

  if (mPosAttribute == nullptr) {
    mPosAttribute = mHeap.Allocate<slang::Attribute>(
        pos_it->attribute.location, slang::ConvertScalarType(pos_it->attribute.format), "in_" + pos_it->name, true);
    mVertexInputState.emplace_back(mPosAttribute);
  }

  BuildVertexProgram();

  BuildFragmentProgram();

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
    auto pos_attr_convert = mHeap.Allocate<slang::TypeConvertOperation>(slang::ScalarType::kVec4,
                                                                        mPosAttribute->GetScalarType(), mPosAttribute);
    auto pos_assign = mHeap.Allocate<slang::BinaryOperation>(
        slang::Operator::kEqual, mHeap.Allocate<slang::NamedNode>("gl_Position"), pos_attr_convert);

    pos_out->AddExpression(pos_assign);
  }

  vs_main->AddStatement(pos_out);

  mVertexShader.SetMainFunc(vs_main);
}

void ShaderGenerator::BuildFragmentProgram() {
  for (const auto& o : mStageShareState) {
    auto i = mHeap.Allocate<slang::Attribute>(o->GetLocation(), o->GetScalarType(), o->GetName(), true);
    mFragmentShader.AddToGlobalScope(i);
  }

  // color output
  auto out_color = mHeap.Allocate<slang::Attribute>(0, slang::ScalarType::kVec4, "FragColor", false);
  mFragmentShader.AddToGlobalScope(out_color);

  // material set
  mFragmentShader.AddToGlobalScope(mHeap.Allocate<slang::StringUniformSet>(mMaterial->GenResourceSet(0)));

  auto custom_func = mHeap.Allocate<slang::StringFunction>("CustomColorFunction", mMaterial->GenColorFunction());

  mFragmentShader.AddToGlobalScope(custom_func);

  auto fs_main = mHeap.Allocate<slang::StatementFunction>("main");

  {
    auto out_assign = mHeap.Allocate<slang::BinaryOperation>(slang::Operator::kEqual, out_color,
                                                             mHeap.Allocate<slang::CallOperation>(custom_func));

    auto statement = mHeap.Allocate<slang::Statement>();
    statement->AddExpression(out_assign);

    fs_main->AddStatement(statement);
  }
  mFragmentShader.SetMainFunc(fs_main);
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

uint32_t get_attr_offset(const std::vector<BufferAttribute>& attrs, const std::string& name) {
  for (uint32_t i = 0; i < attrs.size(); i++) {
    if (attrs[i].name == name) {
      attrs[i].attribute.offset;
    }
  }

  return 0;
}

VertexFormat get_attr_format(const std::vector<BufferAttribute>& attrs, const std::string& name) {
  for (uint32_t i = 0; i < attrs.size(); i++) {
    if (attrs[i].name == name) {
      return attrs[i].attribute.format;
    }
  }

  return VertexFormat::kFloat32;
}

std::vector<VertexBufferLayout> ShaderGenerator::GenVertexBufferLayout() {
  uint32_t stride = 0;

  for (const auto& attr : mGeometry->attribute) {
    switch (attr.attribute.format) {
      case VertexFormat::kFloat32:
        stride += 1 * sizeof(float);
        break;
      case VertexFormat::kFloat32x2:
        stride += 2 * sizeof(float);
        break;
      case VertexFormat::kFloat32x3:
        stride += 3 * sizeof(float);
        break;
      case VertexFormat::kFloat32x4:
        stride += 4 * sizeof(float);
        break;
      default:
        break;
    }
  }

  VertexBufferLayout layout;
  layout.stride = stride;
  layout.stepMode = VertexStepMode::kVertex;

  for (auto const& input : mVertexInputState) {
    VertexAttribute attr{};
    attr.location = input->GetLocation();
    attr.offset = get_attr_offset(mGeometry->attribute, input->GetName());
    attr.format = get_attr_format(mGeometry->attribute, input->GetName());

    layout.attribute.emplace_back(attr);
  }

  return {layout};
}

std::shared_ptr<PipelineLayout> ShaderGenerator::GenPipelineLayout(RenderSystem* renderSystem) {
  // TODO generate bind group in vertex shader stage

  auto material_group = renderSystem->CreateBindGroupLayout(mMaterial->GetResourceInfo());

  return renderSystem->CreatePipelineLayout({material_group});
}

}  // namespace cubic
