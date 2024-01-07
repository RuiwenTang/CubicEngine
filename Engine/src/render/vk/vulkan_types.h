#pragma once

#include <cubic/render/types.h>
#include <volk.h>

namespace cubic {

namespace vk {

TextureFormat TypeConvert(VkFormat format);

}  // namespace vk

}  // namespace cubic
