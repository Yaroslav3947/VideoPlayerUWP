#pragma once

#include "Include.h"
#include <dxgi1_2.h>

class DXHelper {
 public:
  DXHelper(IDXGISwapChain1* swapChain);
  ~DXHelper() = default;

  void Init();

  void RenderBitmapOnWindow(ComPtr<ID2D1Bitmap> pBitmap);
  ComPtr<ID2D1Bitmap> CreateBitmapFromVideoSample(IMFSample* pSample,
                                                  const UINT32& width,
                                                  const UINT32& height);
  void ResizeRenderTarget(const UINT32& width, const UINT32& height);

  std::mutex& GetResizeMtx() { return m_resize_mtx; }

 private:

  std::mutex m_resize_mtx;

  IDXGISwapChain1*  m_swapChain;

  ComPtr<ID2D1Factory1> m_factory;
  ComPtr<ID2D1RenderTarget> m_renderTarget;

  D2D1_SIZE_F m_widgetSize;
};