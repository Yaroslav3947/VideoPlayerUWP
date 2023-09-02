#pragma once

#include "pch.h"

#include "../VideoPlayerStatic/VideoPlayer.h"

namespace VideoPlayerWrapper {
[Windows::Foundation::Metadata::WebHostHidden] public ref class DXGraphics sealed
    : public Windows::UI::Xaml::Controls::SwapChainPanel {
 public:
  DXGraphics();
  void StartVideoPlayer();
  void PlayPauseVideo();
  void OpenURL(Platform::String ^ sURL);
  void SetPosition(Windows::Foundation::TimeSpan position);
  long long GetDuration();
  bool GetIsPaused() { return m_videoPlayer->GetIsPaused(); }

 private
 protected:

  void CreateDeviceIndependentResources();
  void CreateDeviceResources();
  void CreateSizeDependentResources();

  void OnDeviceLost();

  void OnSizeChanged(Platform::Object ^ sender, Windows::UI::Xaml::SizeChangedEventArgs ^ e);
  void OnCompositionScaleChanged(Windows::UI::Xaml::Controls::SwapChainPanel ^ sender,
                                 Platform::Object ^ args);

  ComPtr<IDXGIOutput> m_dxgiOutput;
  ComPtr<ID3D11Device1> m_d3dDevice;
  ComPtr<ID3D11DeviceContext1> m_d3dContext;
  ComPtr<IDXGISwapChain1> m_swapChain;
  //IDXGISwapChain1* m_swapChain;
  ComPtr<ID2D1Factory2> m_d2dFactory;
  ComPtr<ID2D1Device> m_d2dDevice;

  Concurrency::critical_section m_criticalSection;

  float m_renderTargetHeight;
  float m_renderTargetWidth;

  float m_compositionScaleX;
  float m_compositionScaleY;

  float m_height;
  float m_width;

  bool m_loadingComplete = false;

 private:
  VideoPlayer* m_videoPlayer;

  ~DXGraphics();

};

}  // namespace VideoPlayerWrapper
