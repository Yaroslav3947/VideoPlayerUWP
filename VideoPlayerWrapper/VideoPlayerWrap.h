#pragma once

#include "pch.h"

#include "../VideoPlayerWinrtStatic/VideoPlayer.h"

using namespace Windows::Storage::Streams;

namespace VideoPlayerWrapper {
    ref class VideoPlayerWrap;
public
delegate void VideoPlayerPositionChangedHandler(VideoPlayerWrap ^ sender,
                                                long long newVideoPlayerPosition);
public
delegate void VideoPlayerEndOfStreamHandler(VideoPlayerWrap ^ sender);

[Windows::Foundation::Metadata::WebHostHidden] public ref class VideoPlayerWrap sealed
    : public Windows::UI::Xaml::Controls::SwapChainPanel {
 public:
  VideoPlayerWrap();
  void OpenURL(IRandomAccessStream ^videoStreamData);

  // Playback methods
  void Play();
  void Pause();
  long long GetDuration();
  void SetPosition(long long position);

  // Audio methods
  void Mute();
  void Unmute();
  bool GetIsMuted();
  bool GetIsPlaying();
  void ChangeVolume(double volume);

  // SwapChainPanel event handlers
  void ResizeSwapChainPanel(int width, int height, bool isPaused);

  event VideoPlayerPositionChangedHandler ^ VideoPlayerPositionChanged;
  event VideoPlayerEndOfStreamHandler ^ VideoPlayerEndOfStream;

 private
 protected:
     // DirectX Core Objects
  void CreateDeviceResources();
  void CreateSizeDependentResources();
  void CreateDeviceIndependentResources();

  void OnDeviceLost();

  ComPtr<ID2D1Device> m_d2dDevice;
  ComPtr<IDXGIOutput> m_dxgiOutput;
  ComPtr<ID3D11Device1> m_d3dDevice;
  ComPtr<ID2D1Factory2> m_d2dFactory;
  ComPtr<IDXGISwapChain1> m_swapChain;
  ComPtr<ID3D11DeviceContext1> m_d3dContext;

  Concurrency::critical_section m_criticalSection;

  float m_height;
  float m_width;

  bool m_loadingComplete = false;

 private:
  VideoPlayer* m_videoPlayer;

  ~VideoPlayerWrap();
};
}  // namespace VideoPlayerWrapper
