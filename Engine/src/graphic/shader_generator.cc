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

      if (it->name != "position" && it->name != "normal") {
        auto statement = mHeap.Allocate<slang::Statement>();
        // position and normal need to multiply by model matrix if needed
        statement->AddExpression(mHeap.Allocate<slang::BinaryOperation>(
            slang::Operator::kEqual, out_attr,
            mHeap.Allocate<slang::TypeConvertOperation>(out_attr->GetScalarType(), in_attr->GetScalarType(), in_attr)));
        mStageShareStatement.emplace_back(statement);
      }

      if (it->name == "position") {
        mPosAttribute = in_attr;
      }

      out_index++;
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

  slang::UniformBinding* model_matrix_binding = nullptr;

  // other uniform set
  if (mConfigration.has_model_matrix) {
    if (mVertexUniformSlot == -1) {
      mVertexUniformSlot = 0;
    }

    uint32_t uniform_index = 0;
    auto uniform_set = mHeap.Allocate<slang::UniformSet>(mVertexUniformSlot, uniform_index, "ModelMatrix");

    model_matrix_binding = mHeap.Allocate<slang::UniformBinding>(slang::ScalarType::kMat4, "matrix");

    uniform_set->AddBinding(model_matrix_binding);

    mVertexShader.AddToGlobalScope(uniform_set);
  }

  auto vs_main = mHeap.Allocate<slang::StatementFunction>("main");

  auto it = std::find_if(mStageShareState.begin(), mStageShareState.end(),
                         [](slang::Attribute* attr) { return std::strcmp(attr->GetName(), "position") == 0; });
  if (it != mStageShareState.end()) {
    // need to generate position output statement

    auto statement = mHeap.Allocate<slang::Statement>();

    if (model_matrix_binding) {
      auto ts = mHeap.Allocate<slang::BinaryOperation>(
          slang::Operator::kMultiply, model_matrix_binding,
          mHeap.Allocate<slang::TypeConvertOperation>(slang::ScalarType::kVec4, mPosAttribute->GetScalarType(),
                                                      mPosAttribute));

      auto v_pos = mHeap.Allocate<slang::Declaration>("v_pos", slang::ScalarType::kVec4, ts);

      statement->AddExpression(mHeap.Allocate<slang::BinaryOperation>(
          slang::Operator::kEqual, *it,
          mHeap.Allocate<slang::TypeConvertOperation>((*it)->GetScalarType(), slang::ScalarType::kVec4, v_pos)));

      mStageShareStatement.emplace_back(v_pos);
    } else {
      statement->AddExpression(mHeap.Allocate<slang::BinaryOperation>(
          slang::Operator::kEqual, *it,
          mHeap.Allocate<slang::TypeConvertOperation>((*it)->GetScalarType(), mPosAttribute->GetScalarType(),
                                                      mPosAttribute)));
    }

    mStageShareStatement.emplace_back(statement);
  }

  it = std::find_if(mStageShareState.begin(), mStageShareState.end(),
                    [](slang::Attribute* attr) { return std::strcmp(attr->GetName(), "normal") == 0; });

  if (it != mStageShareState.end()) {
    // need to generate normal output statement

    auto statement = mHeap.Allocate<slang::Statement>();

    auto it_in = std::find_if(mVertexInputState.begin(), mVertexInputState.end(),
                              [](slang::Attribute* attr) { return std::strcmp(attr->GetName(), "in_normal") == 0; });

    if (model_matrix_binding) {
      auto ts = mHeap.Allocate<slang::BinaryOperation>(
          slang::Operator::kMultiply, model_matrix_binding,
          mHeap.Allocate<slang::TypeConvertOperation>(slang::ScalarType::kVec4, (*it_in)->GetScalarType(), *it_in,
                                                      true));

      auto v_normal = mHeap.Allocate<slang::Declaration>("v_normal", slang::ScalarType::kVec4, ts);

      statement->AddExpression(mHeap.Allocate<slang::BinaryOperation>(
          slang::Operator::kEqual, *it,
          mHeap.Allocate<slang::TypeConvertOperation>((*it)->GetScalarType(), slang::ScalarType::kVec4, v_normal)));

      mStageShareStatement.emplace_back(v_normal);
    } else {
      statement->AddExpression(mHeap.Allocate<slang::BinaryOperation>(
          slang::Operator::kEqual, *it,
          mHeap.Allocate<slang::TypeConvertOperation>((*it)->GetScalarType(), (*it_in)->GetScalarType(), *it_in)));
    }

	mStageShareStatement.emplace_back(statement);
  }

  for (auto statemen : mStageShareStatement) {
    vs_main->AddStatement(statemen);
  }

  auto pos_out = mHeap.Allocate<slang::Statement>();

  {
    slang::Node* pos_attr_convert = mHeap.Allocate<slang::TypeConvertOperation>(
        slang::ScalarType::kVec4, mPosAttribute->GetScalarType(), mPosAttribute);

    if (model_matrix_binding) {
      pos_attr_convert =
          mHeap.Allocate<slang::BinaryOperation>(slang::Operator::kMultiply, model_matrix_binding, pos_attr_convert);
    }

    auto pos_assign = mHeap.Allocate<slang::BinaryOperation>(
        slang::Operator::kEqual, mHeap.Allocate<slang::NamedNode>("gl_Position"), pos_attr_convert);

    pos_out->AddExpression(pos_assign);
  }

  vs_main->AddStatement(pos_out);

  mVertexShader.SetMainFunc(vs_main);
}

void ShaderGenerator::BuildFragmentProgram() {
  if (mVertexUniformSlot == -1) {
    mVertexUniformSlot = 0;
  } else {
    mFragmentUniformSlot = 1;
  }

  for (const auto& o : mStageShareState) {
    auto i = mHeap.Allocate<slang::Attribute>(o->GetLocation(), o->GetScalarType(), o->GetName(), true);
    mFragmentShader.AddToGlobalScope(i);
  }

  // color output
  auto out_color = mHeap.Allocate<slang::Attribute>(0, slang::ScalarType::kVec4, "FragColor", false);
  mFragmentShader.AddToGlobalScope(out_color);

  // material set
  mFragmentShader.AddToGlobalScope(
      mHeap.Allocate<slang::StringUniformSet>(mMaterial->GenResourceSet(mFragmentUniformSlot)));

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

uint64_t get_attr_offset(const std::vector<BufferAttribute>& attrs, const uint32_t location) {
  for (uint32_t i = 0; i < attrs.size(); i++) {
    if (attrs[i].attribute.location == location) {
      return attrs[i].attribute.offset;
    }
  }

  return 0;
}

VertexFormat get_attr_format(const std::vector<BufferAttribute>& attrs, const uint32_t location) {
  for (uint32_t i = 0; i < attrs.size(); i++) {
    if (attrs[i].attribute.location == location) {
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
    attr.offset = get_attr_offset(mGeometry->attribute, input->GetLocation());
    attr.format = get_attr_format(mGeometry->attribute, input->GetLocation());

    layout.attribute.emplace_back(attr);
  }

  return {layout};
}

std::shared_ptr<PipelineLayout> ShaderGenerator::GenPipelineLayout(RenderSystem* renderSystem) {
  std::vector<std::shared_ptr<BindGroupLayout>> group_layouts = {};

  if (mConfigration.has_model_matrix) {
    group_layouts.emplace_back(renderSystem->CreateBindGroupLayout({GroupEntryInfo{
        0,
        EntryType::kUniformBuffer,
        ShaderStage::kVertexShader,
    }}));
  }

  group_layouts.emplace_back(renderSystem->CreateBindGroupLayout(mMaterial->GetResourceInfo()));

  return renderSystem->CreatePipelineLayout(std::move(group_layouts));
}

}  // namespace cubic
