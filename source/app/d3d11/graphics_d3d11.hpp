#pragma once

#include "app/graphics.hpp"
#include "core/platform/platform.hpp"

// ========================================================================== //
// GraphicsD3D11 Declaration
// ========================================================================== //

namespace dib {

class GraphicsD3D11 : public GraphicsImpl
{
private:
  /** DXGI factory **/
  IDXGIFactory4* mFactory = nullptr;
  /** Adapter **/
  IDXGIAdapter1* mAdapter = nullptr;

  /** Supported feature level **/
  D3D_FEATURE_LEVEL mFeatureLevel;

  /** Device **/
  ID3D11Device3* mDevice = nullptr;
  /** Device context **/
  ID3D11DeviceContext4* mDeviceContext = nullptr;
  /** Device debug interface **/
  ID3D11Debug* mDeviceDebug = nullptr;
  /** Device name **/
  String mDeviceName;

  struct
  {
    /** Swap chain handle **/
    IDXGISwapChain1* handle = nullptr;
    /** Buffer format **/
    DXGI_FORMAT dxgiFormat;

    /** Render-target view **/
    ID3D11RenderTargetView* rtv = nullptr;
    /** Depth-stencil buffer **/
    ID3D11Texture2D* depthStencil = nullptr;
    /** Depth-stencil view **/
    ID3D11DepthStencilView* dsv = nullptr;

    /** Viewport **/
    D3D11_VIEWPORT viewport;
  } mSwapChain;

  /** Clear color **/
  union
  {
    struct
    {
      f32 r, g, b, a;
    };
    f32 rgba[4];
  } mClearColor;

public:
  GraphicsD3D11(const Graphics::Descriptor& descriptor);

  ~GraphicsD3D11();

  const String& DeviceName() const override { return mDeviceName; }

  virtual void SetClearColor(f32 r, f32 g, f32 b, f32 a);

  void Clear() override;

  void Present() override;

private:
  void CreateFactory();

  void CreateDevice();

  bool FindAdapter();

  void CreateSwapChain(const Graphics::Descriptor& descriptor);

  void ResizeSwapChain(u32 width, u32 height);
};

}