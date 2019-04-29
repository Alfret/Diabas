#include "app/d3d11/graphics_d3d11.hpp"

#include "SDL_syswm.h"
#include "core/assert.hpp"

// ========================================================================== //
// GraphicsD3D11 Implementation
// ========================================================================== //

//TODO @Filip fix
#if defined(DIB_TAREGT_WINDOWS)
namespace dib {

GraphicsD3D11::GraphicsD3D11(const Graphics::Descriptor& descriptor)
{
  // Setup context
  CreateFactory();
  CreateDevice();

  // Create swap chain
  CreateSwapChain(descriptor);
  SetClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // Set render target
  mDeviceContext->OMSetRenderTargets(1, &mSwapChain.rtv, nullptr);
}

// -------------------------------------------------------------------------- //

GraphicsD3D11::~GraphicsD3D11()
{
  mAdapter->Release();
  mDeviceContext->Release();
  mDevice->Release();
  mFactory->Release();

#if defined(DIB_DEBUG)
  if (mDeviceDebug) {
    // context->deviceDebug->lpVtbl->ReportLiveDeviceObjects(
    //	context->deviceDebug, D3D11_RLDO_DETAIL);
    mDeviceDebug->Release();
  }
#endif
}

// -------------------------------------------------------------------------- //

void
GraphicsD3D11::SetClearColor(f32 r, f32 g, f32 b, f32 a)
{
  mClearColor.r = r;
  mClearColor.g = g;
  mClearColor.b = b;
  mClearColor.a = a;
}

// -------------------------------------------------------------------------- //

void
GraphicsD3D11::Clear()
{
  mDeviceContext->ClearRenderTargetView(mSwapChain.rtv, mClearColor.rgba);
}

// -------------------------------------------------------------------------- //

void
GraphicsD3D11::Present()
{
  mSwapChain.handle->Present(0, 0);
}

// -------------------------------------------------------------------------- //


void
GraphicsD3D11::CreateFactory()
{
  UINT factoryFlags = 0;
  HRESULT hresult;

#if defined(DIB_DEBUG)
  // Set message severity to break on
  IDXGIInfoQueue* infoQueue;
  hresult = DXGIGetDebugInterface1(0, __uuidof(IDXGIInfoQueue), (void**)&infoQueue);
  if (SUCCEEDED(hresult)) {
    infoQueue->SetBreakOnSeverity(
      DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, TRUE);
    infoQueue->SetBreakOnSeverity(
      DXGI_DEBUG_ALL,
      DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION,
      TRUE);
    infoQueue->Release();
  }

  // Report live objects
  IDXGIDebug1* dxgiDebug;
  hresult = DXGIGetDebugInterface1(
	  0, __uuidof(IDXGIDebug1), (void**)&dxgiDebug);
  if (SUCCEEDED(hresult)) {
    dxgiDebug->ReportLiveObjects(
      DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
    dxgiDebug->Release();
  }

  factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

  // Create factory
  hresult = CreateDXGIFactory2(
    factoryFlags, __uuidof(IDXGIFactory4), (void**)&mFactory);
  DIB_ASSERT(SUCCEEDED(hresult), "Failed to create DXGI factory");
}

// -------------------------------------------------------------------------- //

void
GraphicsD3D11::CreateDevice()
{
  UINT flags = 0;
#if defined(AN_DEBUG)
  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  // Setup requested feature levels in order of most to least wanted version.
  const D3D_FEATURE_LEVEL requestedFeatureLevels[] = { D3D_FEATURE_LEVEL_11_1,
                                                       D3D_FEATURE_LEVEL_11_0 };

  // Find a device
  const bool foundAdapter = FindAdapter();
  if (!foundAdapter) {
    return;
  }

  // Create Device
  HRESULT hresult = D3D11CreateDevice((IDXGIAdapter*)mAdapter,
                                      D3D_DRIVER_TYPE_UNKNOWN,
                                      nullptr,
                                      flags,
                                      requestedFeatureLevels,
                                      2,
                                      D3D11_SDK_VERSION,
                                      (ID3D11Device**)&mDevice,
                                      &mFeatureLevel,
                                      (ID3D11DeviceContext**)&mDeviceContext);
  DIB_ASSERT(SUCCEEDED(hresult), "Failed to create D3D11 device");

  // Retrieve debug interface in debug builds
#if defined(DIB_DEBUG)
  hresult =
    mDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&mDeviceDebug);
  if (SUCCEEDED(hresult)) {
    // context->deviceDebug->lpVtbl->ReportLiveDeviceObjects(
    //	context->deviceDebug, D3D11_RLDO_DETAIL);
  }
#endif
}

// -------------------------------------------------------------------------- //

bool
GraphicsD3D11::FindAdapter()
{
  IDXGIAdapter1* adapter;
  u32 index = 0;
  while (mFactory->EnumAdapters1(index++, &adapter) != DXGI_ERROR_NOT_FOUND) {
    DXGI_ADAPTER_DESC1 adapterDesc;
    adapter->GetDesc1(&adapterDesc);

    // Make sure we are not getting a software device
    const bool isSoftwareAdapter =
      adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE;
    if (isSoftwareAdapter) {
      adapter->Release();
      continue;
    }

    // Choose adapter and retrieve name
    mAdapter = adapter;
    mDeviceName = String((u16*)adapterDesc.Description);
    return true;
  }
  return false;
}

// -------------------------------------------------------------------------- //

void
GraphicsD3D11::CreateSwapChain(const Graphics::Descriptor& descriptor)
{
  // Retrieve window dimensions
  int width, height;
  SDL_GetWindowSize(descriptor.window, &width, &height);

  // Retrieve the window handle
  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(descriptor.window, &info);
  HWND windowHandle = info.info.win.window;

  // Set format
  mSwapChain.dxgiFormat = DXGI_FORMAT_B8G8R8A8_UNORM;

  // Setup create info
  DXGI_SWAP_CHAIN_DESC1 swapChainDesc = { 0 };
  swapChainDesc.Width = width;
  swapChainDesc.Height = height;
  swapChainDesc.Format = mSwapChain.dxgiFormat;
  swapChainDesc.Stereo = false;
  swapChainDesc.SampleDesc.Count = 1;
  swapChainDesc.SampleDesc.Quality = 0;
  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDesc.BufferCount = 3;
  swapChainDesc.Scaling = DXGI_SCALING_NONE;
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
  swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
  swapChainDesc.Flags = 0;

  // Create swap chain
  const HRESULT hresult =
    mFactory->CreateSwapChainForHwnd(
      (IUnknown*)mDevice,
      windowHandle,
      &swapChainDesc,
      nullptr,
      nullptr,
      &mSwapChain.handle
  );
  DIB_ASSERT(SUCCEEDED(hresult), "Failed to create D3D11 swap chain");

  // Do initial resize
  ResizeSwapChain(swapChainDesc.Width, swapChainDesc.Height);
}

// -------------------------------------------------------------------------- //

void
GraphicsD3D11::ResizeSwapChain(u32 width, u32 height)
{
  // Release views and textures
  if (mSwapChain.dsv) {
    mSwapChain.dsv->Release();
  }
  if (mSwapChain.depthStencil) {
    mSwapChain.depthStencil->Release();
  }
  if (mSwapChain.rtv) {
    mSwapChain.rtv->Release();
  }

  // Resize the buffers
  HRESULT result = mSwapChain.handle->ResizeBuffers(
	  0, width, height, mSwapChain.dxgiFormat, 0);
  DIB_ASSERT(SUCCEEDED(result), "Failed to resize swap chain buffers");

  // Get the backbuffer texture
  ID3D11Texture2D* backbufferTexture;
  result = mSwapChain.handle->GetBuffer(
    0, __uuidof(ID3D11Texture2D), (void**)&backbufferTexture);
  DIB_ASSERT(SUCCEEDED(result), "Failed to query swap chain backbuffer");

  // Create the render target view
  mDevice->CreateRenderTargetView(
    (ID3D11Resource*)backbufferTexture,
    nullptr,
    &mSwapChain.rtv);
  DIB_ASSERT(SUCCEEDED(result), "Failed to create render-target view");

  // Release the reference to backbuffer texture
  backbufferTexture->Release();

  // Create the depth stencil texture and view
  D3D11_TEXTURE2D_DESC depthStencilDesc = { 0 };
  depthStencilDesc.Width = width;
  depthStencilDesc.Height = height;
  depthStencilDesc.MipLevels = 1;
  depthStencilDesc.ArraySize = 1;
  depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilDesc.SampleDesc.Count = 1;
  depthStencilDesc.SampleDesc.Quality = 0;
  depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
  depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthStencilDesc.CPUAccessFlags = 0;
  depthStencilDesc.MiscFlags = 0;

  result = mDevice->CreateTexture2D(
    &depthStencilDesc,
    nullptr,
    &mSwapChain.depthStencil);
  DIB_ASSERT(SUCCEEDED(result), "Failed to create depth-stencil texture");

  result = mDevice->CreateDepthStencilView(
    (ID3D11Resource*)mSwapChain.depthStencil,
    nullptr,
    &mSwapChain.dsv);
  DIB_ASSERT(SUCCEEDED(result), "Failed to create depth-stencil view");

  // Update the viewport
  mSwapChain.viewport.TopLeftX = 0;
  mSwapChain.viewport.TopLeftY = 0;
  mSwapChain.viewport.Width = (f32)width;
  mSwapChain.viewport.Height = (f32)height;
  mSwapChain.viewport.MinDepth = 0.0f;
  mSwapChain.viewport.MaxDepth = 1.0f;
}

}
#endif
