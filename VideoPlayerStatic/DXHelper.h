#pragma once

#include "Include.h"

class DXHelper {
 public:
  DXHelper(ComPtr<IDXGISwapChain1> swapChain);
  ~DXHelper() = default;

  void Init();

  void RenderBitmapOnWindow(ComPtr<ID2D1Bitmap> pBitmap);
  ComPtr<ID2D1Bitmap> CreateBitmapFromVideoSample(IMFSample* pSample,
                                                  const UINT32& width,
                                                  const UINT32& height);
  void ResizeRenderTarget(const UINT32& width, const UINT32& height, bool isPaused);

  std::mutex& GetResizeMtx() { return m_resize_mtx; }

 private:
  std::mutex m_resize_mtx;

  ComPtr<IDXGISwapChain1> m_swapChain;

  ComPtr<ID2D1Bitmap> m_lastBitmap;
  ComPtr<ID2D1Factory1> m_factory;
  ComPtr<ID2D1RenderTarget> m_renderTarget;

  D2D1_SIZE_F m_widgetSize;
};