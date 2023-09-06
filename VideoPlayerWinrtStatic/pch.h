#pragma once

#include "targetver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif


#include <Mfidl.h>
#include <Windows.h>
#include <d2d1.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <d3d11.h>
#include <mfapi.h>
#include <mfreadwrite.h>
#include <shlwapi.h>
#include <winrt/base.h>
#include <wrl/client.h>
#include <xaudio2.h>
#include <propsys.h>

#include <mutex>
#include <functional>
#include <propvarutil.h>

using Microsoft::WRL::ComPtr;
