#pragma once

#include "pch.h"

#include "../VideoPlayerStatic/VideoPlayer.h"

#include "DXGraphics.h"

namespace VideoPlayerWrapper {
public
ref class VideoPlayerWrp sealed {
 public:
  VideoPlayerWrp(DXGraphics ^ swapChainPanel);

  void PlayPauseVideo();
  void OpenURL(Platform::String ^ sURL);
  void SetPosition(Windows::Foundation::TimeSpan position);
  long long GetDuration();
  bool GetIsPaused() { return m_pVideoPlayer->GetIsPaused(); }

 private:
  VideoPlayer* m_pVideoPlayer;
};

}  // namespace VideoPlayerWrapper