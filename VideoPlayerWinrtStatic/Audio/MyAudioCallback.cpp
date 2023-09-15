#include "pch.h"
#include "MyAudioCallback.h"

void __stdcall MyAudioCallback::OnStreamEnd() {
  SetEvent(hBufferEndEvent);
  std::lock_guard<std::mutex> lock(m_audioPlaybackMutex);
}
