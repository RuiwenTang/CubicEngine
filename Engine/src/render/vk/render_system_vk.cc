#include "render/vk/render_system_vk.h"

#include <cubic/core/log.h>

namespace cubic {

std::unique_ptr<RenderSystemVk> RenderSystemVk::Create() { return std::make_unique<RenderSystemVk>(); }

RenderSystemVk::~RenderSystemVk() {
  if (mInstance) {
    vkDestroyInstance(mInstance, nullptr);
  }
}

bool RenderSystemVk::Init() {
  if (volkInitialize() != VK_SUCCESS) {
    CUB_ERROR("Failed load vulkan driver !!");
    return false;
  }

  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pEngineName = "Cubic Engine";
  app_info.pApplicationName = "Cubic";
  app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
  app_info.apiVersion = VK_API_VERSION_1_3;

  VkInstanceCreateInfo instance_desc{};
  instance_desc.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_desc.pApplicationInfo = &app_info;

  if (vkCreateInstance(&instance_desc, nullptr, &mInstance) != VK_SUCCESS) {
    CUB_ERROR("Failed create vulkan instance !!");

    return false;
  }

  volkLoadInstance(mInstance);

  return true;
}

}  // namespace cubic
