#pragma once

#include "pch.h"

#include "xaudio2.h"

#pragma comment(lib, "xaudio2.lib")

class MyAudioCallback: public IXAudio2VoiceCallback {
 public:
  MyAudioCallback(std::mutex &audioPlaybackMutex) : 
      m_audioPlaybackMutex(audioPlaybackMutex),
      hBufferEndEvent(CreateEvent(NULL, FALSE, FALSE, NULL)) {}


  ~MyAudioCallback() { CloseHandle(hBufferEndEvent); }

  HANDLE hBufferEndEvent;
  // Called when the voice has just finished playing a contiguous audio stream.
  void STDMETHODCALLTYPE OnStreamEnd() override;

  // Unused methods are stubs
  void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
  void STDMETHODCALLTYPE
  OnVoiceProcessingPassStart(UINT32 SamplesRequired) override {}
  void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override {}
  void STDMETHODCALLTYPE OnBufferStart(void* pBufferContext) override  {}
  void STDMETHODCALLTYPE OnLoopEnd(void* pBufferContext) override {}
  void STDMETHODCALLTYPE OnVoiceError(void* pBufferContext,
                                      HRESULT Error) override {}

 private:
    std::mutex& m_audioPlaybackMutex;
};
