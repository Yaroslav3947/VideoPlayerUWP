#pragma once

#include "pch.h"
#include "Audio/Audio.h"
#include "Audio/MediaReader.h"
#include "Audio/SoundEffect.h"
#include "DXHelper.h"

#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "propsys.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")

////TODO: use Pimpl to not include c++ inlcudes

class VideoPlayer : public IMFAsyncCallback, public IMFSourceReaderCallback {
 public:
  VideoPlayer(ComPtr<IDXGISwapChain1> swapChain,
              std::function<void(long long)> positionChangedCallback,
              std::function<void()> endOfStreamCallback);

  virtual ~VideoPlayer();

  void InitAudio();
  void StartPlayback();
  void InitAudioAndVideoTypes();
  void OpenURL(ComPtr<IMFByteStream> videoDataStream);
  void InitReader(ComPtr<IMFByteStream> videoDataStream);
  void Init(ComPtr<IDXGISwapChain1> swapChain);

  // Playback
  HRESULT Play();
  HRESULT Pause();
  void RequestNextSample();
  void PlayPauseVideo();
  LONGLONG GetDuration();
  void SetPosition(const LONGLONG& hnsPosition);
  inline bool GetIsPaused() const { return m_isPlaying; }

  // Audio
  void Mute();
  void Unmute();
  bool GetIsMuted() const;
  void ChangeVolume(const float& volume);

  DXHelper* GetDxHelper() const { return m_dxhelper.get(); }

 private:
  float GetFPS();
  HRESULT GetWidthAndHeight();

 protected:
  // IUnknown methods
  STDMETHODIMP_(ULONG) AddRef() override;
  STDMETHODIMP_(ULONG) Release() override;
  STDMETHODIMP QueryInterface(REFIID iid, void** ppv) override;

  // IMFAsyncCallback methods
  STDMETHODIMP OnFlush(DWORD) override { return S_OK; }
  STDMETHODIMP OnEvent(DWORD, IMFMediaEvent*) override { return S_OK; }
  STDMETHODIMP Invoke(IMFAsyncResult* pResult) override { return S_OK; }
  STDMETHODIMP GetParameters(DWORD*, DWORD*) override { return E_NOTIMPL; }

  // IMFSourceReaderCallback methods
  STDMETHODIMP OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex,
                            DWORD dwStreamFlags, LONGLONG llTimestamp,
                            IMFSample* pSample) override;

 private:
  ComPtr<IMFSourceReader> m_reader;

  std::unique_ptr<Audio> m_audio;
  std::unique_ptr<DXHelper> m_dxhelper;
  std::unique_ptr<MediaReader> m_mediaReader;
  std::unique_ptr<SoundEffect> m_soundEffect;

  HWND m_hwnd;
  long m_nRefCount;

  DWORD m_streamIndex = 0;
  bool m_isPlaying = false;
  bool m_sampleRequested = false;
  LONGLONG m_currentPosition = 0;

  float m_fps = 0.0;
  UINT32 m_width = 0;
  UINT32 m_height = 0;

  std::function<void(long long)> m_positionChangedCallback;
  std::function<void()> m_endOfStreamCallback;

  enum class StreamIndex { audioStreamIndex = 0, videoStreamIndex = 1 };
};