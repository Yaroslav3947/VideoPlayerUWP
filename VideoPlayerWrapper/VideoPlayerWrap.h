#pragma once

#include "pch.h"

#include "../VideoPlayerStatic/VideoPlayer.h"

namespace VideoPlayerWrapper {
[Windows::Foundation::Metadata::WebHostHidden] public ref class VideoPlayerWrap sealed
    : public Windows::UI::Xaml::Controls::SwapChainPanel {
 public:
  VideoPlayerWrap();

  void PlayPauseVideo();
  void OpenURL(Platform::String ^ sURL);
  void SetPosition(long long position);
  long long GetDuration();
  bool GetIsPaused() { return m_videoPlayer->GetIsPaused(); }
  void ResizeSwapChainPanel(double width, double height);
  void Mute() { m_videoPlayer->Mute(); };
  void Unmute() { m_videoPlayer->Unmute(); };
  void ChangeVolume(double volume) { m_videoPlayer->ChangeVolume(volume); };
  bool GetIsMuted() { return m_videoPlayer->GetIsMuted(); }
  

 private
 protected:
  void CreateDeviceIndependentResources();
  void CreateDeviceResources();
  void CreateSizeDependentResources();

  void OnDeviceLost();

  ComPtr<IDXGIOutput> m_dxgiOutput;
  ComPtr<ID3D11Device1> m_d3dDevice;
  ComPtr<ID3D11DeviceContext1> m_d3dContext;
  ComPtr<IDXGISwapChain1> m_swapChain;
  ComPtr<ID2D1Factory2> m_d2dFactory;
  ComPtr<ID2D1Device> m_d2dDevice;

  Concurrency::critical_section m_criticalSection;

  float m_height;
  float m_width;

  bool m_loadingComplete = false;

 private:
  VideoPlayer* m_videoPlayer;

  ~VideoPlayerWrap();
};

}  // namespace VideoPlayerWrapper
