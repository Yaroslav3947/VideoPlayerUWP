#include "pch.h"
#include "VideoPlayer.h"

#include <propvarutil.h>

#include <mfobjects.h>


VideoPlayer::VideoPlayer(ComPtr<IDXGISwapChain1> swapChain,
                         std::function<void(long long)> positionChangedCallback,
                         std::function<void()> endOfStreamCallback)
    : m_fps(0.0),
      m_width(0),
      m_height(0),
      m_nRefCount(1),
      m_reader(nullptr),
      m_isPlaying(false),
      m_mediaReader(nullptr),
      m_soundEffect(nullptr),
      m_positionChangedCallback(positionChangedCallback),
      m_endOfStreamCallback(endOfStreamCallback) {
  Init(swapChain);
}

//-----------------------------------------------------------------------------
// IUnknown Methods
//-----------------------------------------------------------------------------

STDMETHODIMP VideoPlayer::QueryInterface(REFIID iid, void **ppv) {
  if (ppv == nullptr) {
    return E_POINTER;
  }

  *ppv = nullptr;

  if (iid == IID_IUnknown || iid == IID_IMFAsyncCallback) {
    *ppv = static_cast<IMFAsyncCallback *>(this);
  } else if (iid == IID_IMFSourceReaderCallback) {
    *ppv = static_cast<IMFSourceReaderCallback *>(this);
  } else {
    return E_NOINTERFACE;
  }

  AddRef();

  return S_OK;
}

ULONG VideoPlayer::AddRef() { return InterlockedIncrement(&m_nRefCount); }

ULONG VideoPlayer::Release() {
  ULONG uCount = InterlockedDecrement(&m_nRefCount);
  if (uCount == 0) {
    delete this;
  }
  return uCount;
}

void VideoPlayer::Init(ComPtr<IDXGISwapChain1> swapChain) {
  winrt::check_hresult(MFStartup(MF_VERSION));

  m_dxhelper = std::make_unique<DXHelper>(swapChain);
  m_mediaReader = std::make_unique<MediaReader>();
  m_soundEffect = std::make_unique<SoundEffect>();
  m_audio = std::make_unique<Audio>();
}

void VideoPlayer::OpenURL(ComPtr<IMFByteStream> videoDataStream) {

  InitReader(videoDataStream);

  m_fps = GetFPS();
  winrt::check_hresult(GetWidthAndHeight());

  InitAudioAndVideoTypes();

  InitAudio();
  StartPlayback();

  return;
}

void VideoPlayer::InitAudio() {
  m_audio->CreateDeviceIndependentResources();

  m_soundEffect->Initialize(m_audio->SoundEffectEngine(),
                            m_mediaReader->GetWaveFormat(m_reader));
}

void VideoPlayer::StartPlayback() {
  m_reader->ReadSample(MF_SOURCE_READER_ANY_STREAM, 0, nullptr, nullptr,
                       nullptr, nullptr);

    m_isPlaying = true;
}

void VideoPlayer::InitReader(ComPtr<IMFByteStream> videoDataStream) {
  m_reader.Reset();

  ComPtr<IMFAttributes> pAttributes;
  winrt::check_hresult(MFCreateAttributes(pAttributes.GetAddressOf(), 1));

  // Enable hardware transforms and video processing
  pAttributes->SetUINT32(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, TRUE);
  pAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);

  // Set the asynchronous callback for the source reader
  winrt::check_hresult(
      pAttributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK,
                              static_cast<IMFSourceReaderCallback *>(this)));

  winrt::check_hresult(MFCreateSourceReaderFromByteStream(
      videoDataStream.Get(), pAttributes.Get(), m_reader.GetAddressOf()));
}

void VideoPlayer::InitAudioAndVideoTypes() {
  ComPtr<IMFMediaType> pAudioType;
  winrt::check_hresult(MFCreateMediaType(&pAudioType));

  pAudioType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
  pAudioType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

  winrt::check_hresult(m_reader->SetCurrentMediaType(
      (DWORD)StreamIndex::audioStreamIndex, nullptr, pAudioType.Get()));

  ComPtr<IMFMediaType> pVideoType;
  MFCreateMediaType(&pVideoType);

  pVideoType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
  pVideoType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);

  winrt::check_hresult(m_reader->SetCurrentMediaType(
      (DWORD)StreamIndex::videoStreamIndex, nullptr, pVideoType.Get()));
}


void VideoPlayer::RequestNextSample() {
  winrt::check_hresult(m_reader->ReadSample(MF_SOURCE_READER_ANY_STREAM, 0,
                                            NULL, NULL, NULL, NULL));
}

HRESULT VideoPlayer::Play() {
  if (!m_isPlaying) {
    m_isPlaying = true;
    RequestNextSample();  
  }
  return S_OK;
}

HRESULT VideoPlayer::Pause() {
  m_isPlaying = false;
  return S_OK;
}

LONGLONG VideoPlayer::GetDuration() {
  LONGLONG duration = 0;

  PROPVARIANT var;
  PropVariantInit(&var);

  HRESULT hr = m_reader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE,
                                                  MF_PD_DURATION, &var);
  if (SUCCEEDED(hr)) {
    duration = var.hVal.QuadPart;
    PropVariantClear(&var);
  }
  return duration;
}

void VideoPlayer::SetPosition(const LONGLONG &hnsNewPosition) {
  if (!m_reader) return;

  PROPVARIANT var;
  PropVariantInit(&var);
  var.vt = VT_I8;
  var.hVal.QuadPart = hnsNewPosition;

   //winrt::check_hresult(m_reader->SetCurrentPosition(GUID_NULL, var));
  HRESULT hr = m_reader->SetCurrentPosition(GUID_NULL, var);

  PropVariantClear(&var);

}

//-----------------------------------------------------------------------------
// Playback Methods
//-----------------------------------------------------------------------------

HRESULT VideoPlayer::GetWidthAndHeight() {
  ComPtr<IMFMediaType> pMediaType;
  HRESULT hr = m_reader->GetCurrentMediaType(1, &pMediaType);
  if (SUCCEEDED(hr)) {
    hr = MFGetAttributeSize(pMediaType.Get(), MF_MT_FRAME_SIZE, &m_width,
                            &m_height);
  }
  return hr;
}

int VideoPlayer::GetFPS() {
  ComPtr<IMFMediaType> pMediaType = nullptr;
  HRESULT hr = m_reader->GetCurrentMediaType(1, &pMediaType);
  if (SUCCEEDED(hr)) {
    UINT32 numerator{0}, denominator{0};
    winrt::check_hresult(MFGetAttributeRatio(pMediaType.Get(), MF_MT_FRAME_RATE,
                                             &numerator, &denominator));

    if (denominator > 0) {
      return static_cast<float>(numerator) / static_cast<float>(denominator);
    }
  }

  return 0;
}

HRESULT VideoPlayer::OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex,
                                  DWORD dwStreamFlags, LONGLONG llTimestamp,
                                  IMFSample *pSample) {
  std::lock_guard<std::mutex> lock(GetDxHelper()->GetResizeMtx());

  if (dwStreamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
    m_endOfStreamCallback();
    m_isPlaying = false;
    return S_OK;
  }

  if (m_isPlaying) {
    if (dwStreamIndex == (DWORD)StreamIndex::videoStreamIndex) {
      ComPtr<ID2D1Bitmap> bitmap;
      bitmap =
          m_dxhelper->CreateBitmapFromVideoSample(pSample, m_width, m_height);
      m_dxhelper->RenderBitmapOnWindow(bitmap);

      static int sampleCounter = 0;

      if (sampleCounter++ % GetFPS() == 0) {
        m_positionChangedCallback(llTimestamp);
      }
    } else if (dwStreamIndex == (DWORD)StreamIndex::audioStreamIndex) {
      auto soundData = m_mediaReader->LoadMedia(pSample);
      {
        std::lock_guard<std::mutex> lock(m_soundEffect->GetAudioPlaybackMutex());
        m_soundEffect->PlaySound(soundData);
      }
    }

    RequestNextSample();
  }

  return S_OK;
}

bool VideoPlayer::GetIsMuted() const { return m_soundEffect->IsMute(); }
void VideoPlayer::Mute() { m_soundEffect->Mute(); }
void VideoPlayer::Unmute() { m_soundEffect->Unmute(); }
void VideoPlayer::ChangeVolume(const float &volume) {
  m_soundEffect->ChangeVolume(volume);
}

VideoPlayer::~VideoPlayer() { MFShutdown(); }
