#pragma once

#include "Include.h"

#include "Audio/Audio.h"
#include "Audio/MediaReader.h"
#include "Audio/SoundEffect.h"

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Propsys.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")


class VideoPlayer : public IMFAsyncCallback, public IMFSourceReaderCallback {
 public:
  VideoPlayer();
  virtual ~VideoPlayer();

  void Init();
  void InitAudio();
  void StartPlayback();
  void InitAudioAndVideoTypes();
  void OpenURL(const WCHAR* sURL);
  void InitReader(const WCHAR* sURL);

  // Playback
  void PlayPauseVideo();

  void SetPosition(const LONGLONG& hnsPosition);

  LONGLONG GetDuration();
  inline bool GetIsPaused() const { return m_isPaused; }

  SoundEffect* GetSoundEffect() const { return m_soundEffect.get(); }
  //DXHelper* GetDxHelper() const { return m_dxhelper.get(); }

 private:
  HRESULT GetWidthAndHeight();
  float GetFPS();

 protected:
  // IUnknown methods
  STDMETHODIMP QueryInterface(REFIID iid, void** ppv) override;
  STDMETHODIMP_(ULONG) AddRef() override;
  STDMETHODIMP_(ULONG) Release() override;

  // IMFAsyncCallback methods
  STDMETHODIMP OnEvent(DWORD, IMFMediaEvent*) override { return S_OK; }
  STDMETHODIMP OnFlush(DWORD) override { return S_OK; }
  STDMETHODIMP GetParameters(DWORD*, DWORD*) override { return E_NOTIMPL; }
  STDMETHODIMP Invoke(IMFAsyncResult* pResult) override { return S_OK; }

  // IMFSourceReaderCallback methods
  STDMETHODIMP OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex,
                            DWORD dwStreamFlags, LONGLONG llTimestamp,
                            IMFSample* pSample) override;

 private:
  ComPtr<IMFSourceReader> m_reader;
  //std::unique_ptr<DXHelper> m_dxhelper;
  std::unique_ptr<MediaReader> m_mediaReader;
  std::unique_ptr<SoundEffect> m_soundEffect;
  std::unique_ptr<Audio> m_audio;

  long m_nRefCount;
  HWND m_hwnd;

  bool m_isPaused = false;
  DWORD m_streamIndex = 0;
  LONGLONG m_currentPosition = 0;

  float m_fps = 0.0;
  UINT32 m_width = 0;
  UINT32 m_height = 0;

  enum class StreamIndex { audioStreamIndex = 0, videoStreamIndex = 1 };
};