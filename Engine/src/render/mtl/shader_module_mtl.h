#import <Metal/Metal.h>
#include <cubic/render/shader_module.h>

namespace cubic {

class ShaderModuleMTL : public ShaderModule {
 public:
  ShaderModuleMTL(ShaderStage stage, std::string label, id<MTLLibrary> shader);

  ~ShaderModuleMTL() override;

  id<MTLLibrary> GetNativeShader() const;

  static std::shared_ptr<ShaderModule> Compile(id<MTLDevice> gpu, ShaderModuleDescriptor* desc,
                                               const std::vector<uint32_t>& spv);

 private:
  id<MTLLibrary> mNativeShader;
};

}  // namespace cubic
