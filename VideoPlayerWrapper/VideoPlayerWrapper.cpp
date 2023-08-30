#include "pch.h"

#include "VideoPlayerWrapper.h"

using namespace VideoPlayerWrapper;

VideoPlayerWrp::VideoPlayerWrp() { m_pVideoPlayer = new VideoPlayer(); }

void VideoPlayerWrp::PlayPauseVideo() { m_pVideoPlayer->PlayPauseVideo(); }

void VideoPlayerWrp::OpenURL(Platform::String ^ sURL) {
  const WCHAR* url = sURL->Data();
  m_pVideoPlayer->OpenURL(url);
}

void VideoPlayerWrp::SetPosition(Windows::Foundation::TimeSpan position) {
  m_pVideoPlayer->SetPosition(position.Duration);
}

long long VideoPlayerWrp::GetDuration() {
  return m_pVideoPlayer->GetDuration();
}