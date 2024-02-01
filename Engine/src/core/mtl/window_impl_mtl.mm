#include "core/mtl/window_impl_mtl.h"
#include <GLFW/glfw3.h>
#include "render/mtl/mtl_types.h"
#include "render/mtl/render_system_info_mtl.h"
#include "render/mtl/texture_mtl.h"
#include "render/render_system_priv.h"

#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>

#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

namespace cubic {

class WindowMTLPriv {
 public:
  WindowMTLPriv(GLFWwindow* nativeWindow) : mNativeWindow(nativeWindow) {}

  ~WindowMTLPriv() = default;

  bool Init(RenderSystemInfoMTL* info) {
    mLayer = [CAMetalLayer new];

    if (mLayer == nil) {
      return false;
    }

    NSWindow* ns_window = glfwGetCocoaWindow(mNativeWindow);

    if (ns_window == nil) {
      return false;
    }

    mLayer.device = MTLCreateSystemDefaultDevice();
    mLayer.opaque = YES;
    mLayer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    mLayer.contentsScale = [[NSScreen mainScreen] backingScaleFactor];
    mLayer.colorspace = CGColorSpaceCreateDeviceRGB();

    ns_window.contentView.layer = mLayer;
    ns_window.contentView.wantsLayer = YES;

    mDevice = info->device;
    mQueue = info->queue;

    return true;
  }

  std::shared_ptr<Texture> AcquireTexture() {
    mDrawable = [mLayer nextDrawable];

    if (mDrawable == nil) {
      return {};
    }

    id<MTLTexture> texture = mDrawable.texture;

    TextureDescirptor desc{};
    desc.format = TypeConvert(mLayer.pixelFormat);
    desc.usage = TextureUsage::kRenderTarget;
    desc.sample_count = 1;
    desc.width = texture.width;
    desc.height = texture.height;

    return std::make_shared<TextureMTL>(desc, texture);
  }

  void SwapWindowBuffer() {
    if (mDrawable == nil) {
      return;
    }

    id<MTLCommandBuffer> cmd = [mQueue commandBuffer];

    [cmd presentDrawable:mDrawable];

    [cmd commit];

    [cmd release];

    mDrawable = nil;
  }

 private:
  GLFWwindow* mNativeWindow;
  id<MTLDevice> mDevice = nil;
  id<MTLCommandQueue> mQueue = nil;
  CAMetalLayer* mLayer = nil;

  id<CAMetalDrawable> mDrawable = nil;
};

WindowImplMTL::WindowImplMTL(WindowProps props, GLFWwindow* nativeWindow, RenderSystem* renderSystem)
    : WindowImpl(std::move(props), nativeWindow, renderSystem) {}

WindowImplMTL::~WindowImplMTL() {}

bool WindowImplMTL::Init() {
  mPriv = std::make_unique<WindowMTLPriv>(GetNativeWindow());

  auto info = dynamic_cast<RenderSystemPriv*>(GetRenderSystem())->GetBackendInfo();

  return mPriv->Init(static_cast<RenderSystemInfoMTL*>(info));
}

void WindowImplMTL::Terminate() {}

std::shared_ptr<Texture> WindowImplMTL::AcquireTexture() { return mPriv->AcquireTexture(); }

void WindowImplMTL::SwapWindowBuffer() { mPriv->SwapWindowBuffer(); }

}  // namespace cubic
