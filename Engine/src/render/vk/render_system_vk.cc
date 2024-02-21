#define VMA_IMPLEMENTATION
// implement
#include <vk_mem_alloc.h>
// before self header

#include <cubic/core/log.h>

#include <vector>

#include "render/vk/bind_group_vk.h"
#include "render/vk/buffer_vk.h"
#include "render/vk/command_queue_vk.h"
#include "render/vk/pipeline_layout_vk.h"
#include "render/vk/render_pipeline_vk.h"
#include "render/vk/render_system_vk.h"
#include "render/vk/shader_module_vk.h"
#include "render/vk/texture_vk.h"

namespace cubic {

VKAPI_ATTR VkBool32 VKAPI_CALL _debug_utils_messenger_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
  if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT ||
      messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
    CUB_INFO("[ {} ][ {} ] : {}", pCallbackData->messageIdNumber, pCallbackData->pMessageIdName,
             pCallbackData->pMessage);
  } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    CUB_WARR("[ {} ][ {} ] : {}", pCallbackData->messageIdNumber, pCallbackData->pMessageIdName,
             pCallbackData->pMessage);
  } else {
    CUB_ERROR("[ {} ][ {} ] : {}", pCallbackData->messageIdNumber, pCallbackData->pMessageIdName,
              pCallbackData->pMessage);
  }

  return VK_FALSE;
}

std::unique_ptr<RenderSystemVk> RenderSystemVk::Create() { return std::make_unique<RenderSystemVk>(); }

RenderSystemVk::~RenderSystemVk() {
  vmaDestroyAllocator(mAllocator);

  mDevice.reset();

  if (mDebugHandler) {
    vkDestroyDebugUtilsMessengerEXT(mInstance, mDebugHandler, nullptr);
  }

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

  if (!initDevice()) {
    CUB_ERROR("No GPU device support Vulkan !!");

    return false;
  }

  mInfo.backend = Backend::kVulkan;
  mInfo.instance = mInstance;
  mInfo.device = mDevice.get();

  VmaAllocatorCreateInfo allocator_info{};
  allocator_info.vulkanApiVersion = VK_API_VERSION_1_3;
  allocator_info.physicalDevice = mDevice->GetPhysicalDevice();
  allocator_info.device = mDevice->GetLogicalDevice();
  allocator_info.instance = mInstance;

  VmaVulkanFunctions vma_functions{};
  vma_functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
  vma_functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

  allocator_info.pVulkanFunctions = &vma_functions;

  if (vmaCreateAllocator(&allocator_info, &mAllocator) != VK_SUCCESS) {
    CUB_ERROR("[Vulkan backend] Failed init memory allocator !!");
    return false;
  }

  return true;
}

RenderSystemInfo* RenderSystemVk::GetBackendInfo() { return &mInfo; }

CommandQueue* RenderSystemVk::GetCommandQueue(QueueType type) {
  switch (type) {
    case QueueType::kGraphic:
      return mDevice->GetGraphicQueue();
    case QueueType::kCompute:
      return mDevice->GetComputeQueue();
    case QueueType::kTransfer:
      return mDevice->GetTransferQueue();
    default:
      return nullptr;
  }
}

uint32_t RenderSystemVk::GetMinBufferAlignment() const { return mDevice->GetMinBufferAlignment(); }

std::shared_ptr<RenderPipeline> RenderSystemVk::CreateRenderPipeline(RenderPipelineDescriptor* desc) {
  return RenderPipelineVK::Create(mDevice.get(), desc);
}

std::shared_ptr<Texture> RenderSystemVk::CreateTexture(TextureDescirptor* desc) {
  return TextureVK::Create(desc, mAllocator, mDevice.get());
}

std::shared_ptr<Buffer> RenderSystemVk::CreateBuffer(BufferDescriptor* desc) {
  return BufferVK::Create(desc, mAllocator);
}

std::shared_ptr<BindGroupLayout> RenderSystemVk::CreateBindGroupLayout(std::vector<GroupEntryInfo> entries) {
  return BindGroupLayoutVK::Create(std::move(entries), mDevice.get());
}

std::shared_ptr<PipelineLayout> RenderSystemVk::CreatePipelineLayout(
    std::vector<std::shared_ptr<BindGroupLayout>> groups) {
  return PipelineLayoutVK::Create(std::move(groups), mDevice.get());
}

std::shared_ptr<BindGroup> RenderSystemVk::CreateBindGroup(const std::shared_ptr<BindGroupLayout>& layout,
                                                           std::vector<GroupEntry> entries) {
  // TODO: implement BindGroup
  return std::shared_ptr<BindGroup>();
}

std::shared_ptr<ShaderModule> RenderSystemVk::CompileBackendShader(ShaderModuleDescriptor* desc,
                                                                   const std::vector<uint32_t>& spv) {
  return ShaderModuleVK::Compile(mDevice.get(), spv, desc);
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

  VkResult ret = vkCreateInstance(&instance_desc, nullptr, &mInstance);

  if (ret != VK_SUCCESS) {
    return false;
  }

  if (enableDebug) {
    setupValidation();
  }

  return true;
}

void RenderSystemVk::setupValidation() {
  if (vkCreateDebugUtilsMessengerEXT == nullptr) {
    vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(mInstance, "vkCreateDebugUtilsMessengerEXT"));
  }

  if (vkDestroyDebugUtilsMessengerEXT == nullptr) {
    vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(mInstance, "vkDestroyDebugUtilsMessengerEXT"));
  }

  VkDebugUtilsMessengerCreateInfoEXT create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
  create_info.pfnUserCallback = _debug_utils_messenger_callback;

  if (vkCreateDebugUtilsMessengerEXT(mInstance, &create_info, nullptr, &mDebugHandler) != VK_SUCCESS) {
    CUB_ERROR("Failed create vulkan debug handler !!");
  }
}

bool RenderSystemVk::initDevice() {
  uint32_t gpuCount = 0;

  vkEnumeratePhysicalDevices(mInstance, &gpuCount, nullptr);

  if (gpuCount == 0) {
    return false;
  }

  std::vector<VkPhysicalDevice> gpuDevice(gpuCount);

  vkEnumeratePhysicalDevices(mInstance, &gpuCount, gpuDevice.data());

  // just pick first device, it should work on almost platform

  mDevice = VulkanDevice::Create(gpuDevice.front());

  return mDevice != nullptr;
}

}  // namespace cubic
