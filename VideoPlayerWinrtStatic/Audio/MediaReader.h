#pragma once

#include "../pch.h"

class MediaReader {
 public:
  MediaReader() = default;

  std::vector<byte> LoadMedia(ComPtr<IMFSample> pSample);
  WAVEFORMATEX *GetWaveFormat(ComPtr<IMFSourceReader> reader);

 private:
  WAVEFORMATEX m_waveFormat;
};