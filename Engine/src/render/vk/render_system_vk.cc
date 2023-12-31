#include "render/vk/render_system_vk.h"

#include <cubic/core/log.h>

#include <vector>

namespace cubic {

std::unique_ptr<RenderSystemVk> RenderSystemVk::Create() { return std::make_unique<RenderSystemVk>(); }

RenderSystemVk::~RenderSystemVk() {
  if (mInstance) {
    vkDestroyInstance(mInstance, nullptr);
  }
}

bool RenderSystemVk::Init(bool enableDebug) {
  if (volkInitialize() != VK_SUCCESS) {
    CUB_ERROR("Failed load vulkan driver !!");
    return false;
  }

  if (!initInstance(enableDebug)) {
    CUB_ERROR("Failed create vulkan instance !!");

    return false;
  }

  volkLoadInstance(mInstance);

  return true;
}

bool RenderSystemVk::initInstance(bool enableDebug) {
  VkApplicationInfo app_info{};
  app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  app_info.pEngineName = "Cubic Engine";
  app_info.pApplicationName = "Cubic";
  app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
  app_info.apiVersion = VK_API_VERSION_1_3;

  VkInstanceCreateInfo instance_desc{};
  instance_desc.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_desc.pApplicationInfo = &app_info;

  std::vector<const char*> instanceExtensions = {VK_KHR_SURFACE_EXTENSION_NAME};

#if defined(CUBIC_PLATFORM_WINDOWS)
  instanceExtensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif

  std::vector<VkExtensionProperties> supportedExtensions{};
  uint32_t extCount = 0;
  vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
  if (extCount > 0) {
    supportedExtensions.resize(extCount);

    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, supportedExtensions.data());
  }

  if (enableDebug) {
    bool findDebugExt = false;
    for (auto const& prop : supportedExtensions) {
      if (std::strcmp(prop.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0) {
        findDebugExt = true;
        break;
      }
    }

    if (!findDebugExt) {
      CUB_ERROR("debugMode is enabled, but can not find debug extension !!");
    } else {
      instanceExtensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
  }

  instance_desc.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
  instance_desc.ppEnabledExtensionNames = instanceExtensions.data();

  std::vector<const char*> requiredLayers{};

  const char* validationLayerName = "VK_LAYER_KHRONOS_validation";

  if (enableDebug) {
    uint32_t layerCount = 0;

    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> instanceLayers{layerCount};

    vkEnumerateInstanceLayerProperties(&layerCount, instanceLayers.data());

    bool layerFound = false;

    for (auto const& layer : instanceLayers) {
      if (std::strcmp(layer.layerName, validationLayerName) == 0) {
        layerFound = true;
        break;
      }
    }

    if (layerFound) {
      requiredLayers.emplace_back(validationLayerName);
    } else {
      CUB_ERROR("debug is enabled, but can not found vulkan validation layer !!");
    }
  }

  instance_desc.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size());
  instance_desc.ppEnabledLayerNames = requiredLayers.data();

  return vkCreateInstance(&instance_desc, nullptr, &mInstance) == VK_SUCCESS;
}

}  // namespace cubic
