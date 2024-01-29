#include "core/vk/swap_chain.h"

#include <cubic/core/log.h>

#include "render/vk/command_buffer_vk.h"
#include "render/vk/command_queue_vk.h"
#include "render/vk/vulkan_device.h"

namespace cubic {

Swapchain::Swapchain(VkInstance instance, VkSurfaceKHR surface, VulkanDevice* device)
    : mInstance(instance), mSurface(surface), mDevice(device) {}

Swapchain::~Swapchain() {
  vkDeviceWaitIdle(mDevice->GetLogicalDevice());

  if (mSwapchain) {
    CleanBuffers();
    vkDestroySwapchainKHR(mDevice->GetLogicalDevice(), mSwapchain, nullptr);
  }
}

bool Swapchain::Resize(uint32_t width, uint32_t height, VkSurfaceFormatKHR format) {
  VkSurfaceCapabilitiesKHR surface_props{};

  if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(mDevice->GetPhysicalDevice(), mSurface, &surface_props) != VK_SUCCESS) {
    CUB_ERROR("Failed query gpu surface property !!");
    return false;
  }

  if (surface_props.currentExtent.width == 0xFFFFFFFF) {
    mSwapchainSize.width = width;
    mSwapchainSize.height = height;
  } else {
    mSwapchainSize = surface_props.currentExtent;
  }

  // supported by all implementations
  VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;

  // swapchain buffer counts
  uint32_t desired_swapchain_images = surface_props.minImageCount + 1;
  if ((surface_props.maxImageCount > 0) && (desired_swapchain_images > surface_props.maxImageCount)) {
    // make sure buffer count less than swapchain max buffer count
    desired_swapchain_images = surface_props.maxImageCount;
  }

  VkSurfaceTransformFlagBitsKHR pre_transform;
  if (surface_props.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
    pre_transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  } else {
    pre_transform = surface_props.currentTransform;
  }

  VkSwapchainKHR old_swapchain = mSwapchain;

  // composite type
  VkCompositeAlphaFlagBitsKHR composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  if (surface_props.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) {
    composite = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  } else if (surface_props.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) {
    composite = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
  } else if (surface_props.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR) {
    composite = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
  } else if (surface_props.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR) {
    composite = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
  }

  VkSwapchainCreateInfoKHR create_info{VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR};
  create_info.surface = mSurface;
  create_info.minImageCount = desired_swapchain_images;
  create_info.imageFormat = format.format;
  create_info.imageColorSpace = format.colorSpace;
  create_info.imageExtent.width = mSwapchainSize.width;
  create_info.imageExtent.height = mSwapchainSize.height;
  create_info.imageArrayLayers = 1;
  create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  create_info.preTransform = pre_transform;
  create_info.compositeAlpha = composite;
  create_info.presentMode = present_mode;
  create_info.clipped = true;
  create_info.oldSwapchain = old_swapchain;

  if (surface_props.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
    create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }

  if (surface_props.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
    create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }

  if (vkCreateSwapchainKHR(mDevice->GetLogicalDevice(), &create_info, nullptr, &mSwapchain) != VK_SUCCESS) {
    CUB_ERROR("Failed create vulkan swapchain !!");
    return false;
  }

  if (old_swapchain) {
    CleanBuffers();

    vkDestroySwapchainKHR(mDevice->GetLogicalDevice(), old_swapchain, nullptr);
  }

  mFormat = format;

  return InitBuffers();
}

SwapchainResult Swapchain::AcquireNextFrame(VkSemaphore signal_semaphore, VkFence signal_fence) {
  auto state = vkAcquireNextImageKHR(mDevice->GetLogicalDevice(), mSwapchain, UINT64_MAX, signal_semaphore,
                                     signal_fence, &mCurrentFrameIndex);

  if (state != VK_SUCCESS) {
    return SwapchainResult(state);
  }

  TextureDescriptorVK desc{};
  desc.format = mFormat.format;
  desc.image = mBuffers[mCurrentFrameIndex].image;
  desc.view = mBuffers[mCurrentFrameIndex].view;
  desc.layout = VK_IMAGE_LAYOUT_UNDEFINED;

  return SwapchainResult(TextureVK::WrapSwapchainTexture(mSwapchainSize.width, mSwapchainSize.height, desc, mDevice),
                         state);
}

VkResult Swapchain::SubmitFrame(CommandQueueVK* queue, std::shared_ptr<TextureVK> texture, VkSemaphore wait_semaphore) {
  if (texture->GetImageLayout() != VK_IMAGE_LAYOUT_PRESENT_SRC_KHR) {
    // maybe has empty render loop or some thing is wrong during the root render pass
    auto cmd = queue->GenCommandBuffer();

    auto vk_cmd = dynamic_cast<CommandBufferVK*>(cmd.get())->GetNativeBuffer();

    VkImageMemoryBarrier image_barrier{};
    image_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    image_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    image_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    image_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    image_barrier.subresourceRange.baseMipLevel = 0;
    image_barrier.subresourceRange.levelCount = 1;
    image_barrier.subresourceRange.baseArrayLayer = 0;
    image_barrier.subresourceRange.layerCount = 1;

    image_barrier.srcAccessMask = 0;
    image_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    image_barrier.oldLayout = texture->GetImageLayout();
    image_barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    image_barrier.image = texture->GetImage();

    vkCmdPipelineBarrier(vk_cmd, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr,
                         1, &image_barrier);

    queue->Submit(std::move(cmd));

    texture->SetImageLayout(VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  }

  VkPresentInfoKHR preseint_info = {};
  preseint_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  preseint_info.pNext = NULL;
  preseint_info.swapchainCount = 1;
  preseint_info.pSwapchains = &mSwapchain;
  preseint_info.pImageIndices = &mCurrentFrameIndex;

  if (wait_semaphore != VK_NULL_HANDLE) {
    preseint_info.pWaitSemaphores = &wait_semaphore;
    preseint_info.waitSemaphoreCount = 1;
  }

  return vkQueuePresentKHR(queue->GetNativeQueue(), &preseint_info);
}

void Swapchain::CleanBuffers() {
  for (const auto& buffer : mBuffers) {
    vkDestroyImageView(mDevice->GetLogicalDevice(), buffer.view, nullptr);
  }
}

bool Swapchain::InitBuffers() {
  uint32_t image_count;
  vkGetSwapchainImagesKHR(mDevice->GetLogicalDevice(), mSwapchain, &image_count, nullptr);

  if (image_count == 0) {
    return false;
  }

  std::vector<VkImage> images(image_count);
  vkGetSwapchainImagesKHR(mDevice->GetLogicalDevice(), mSwapchain, &image_count, images.data());

  mBuffers.resize(image_count);

  for (uint32_t i = 0; i < image_count; i++) {
    mBuffers[i].image = images[i];

    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.format = mFormat.format;
    create_info.components = {
        VK_COMPONENT_SWIZZLE_R,
        VK_COMPONENT_SWIZZLE_G,
        VK_COMPONENT_SWIZZLE_B,
        VK_COMPONENT_SWIZZLE_A,
    };
    create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    create_info.subresourceRange.baseMipLevel = 0;
    create_info.subresourceRange.levelCount = 1;
    create_info.subresourceRange.baseArrayLayer = 0;
    create_info.subresourceRange.layerCount = 1;
    create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    create_info.flags = 0;

    create_info.image = images[i];

    if (vkCreateImageView(mDevice->GetLogicalDevice(), &create_info, nullptr, &mBuffers[i].view) != VK_SUCCESS) {
      return false;
    }
  }

  return true;
}

}  // namespace cubic
