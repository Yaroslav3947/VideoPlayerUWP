#include "pch.h"

#include "DXGraphics.h"


using namespace VideoPlayerWrapper;

using namespace D2D1;
using namespace DX;
using namespace Platform;
using namespace Concurrency;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::Storage::Streams;
using namespace Windows::System::Threading;

static const float m_dipsPerInch = 96.0f;

DXGraphics::DXGraphics()
    : m_compositionScaleX(1.0f),
      m_compositionScaleY(1.0f),
      m_width(1920.0f),
      m_height(1080.0f),
      m_swapChain(nullptr) {

  /*  this->SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &DXGraphics ::OnSizeChanged);
  this->CompositionScaleChanged +=
      ref new Windows::Foundation::TypedEventHandler<SwapChainPanel ^,
                                                     Object ^>(this, &DXGraphics::OnCompositionScaleChanged);*/

  CreateDeviceIndependentResources();
  CreateDeviceResources();
  CreateSizeDependentResources();

  StartVideoPlayer();

}

void DXGraphics::StartVideoPlayer() {
  m_videoPlayer = new VideoPlayer(m_swapChain.Get());
}

void DXGraphics::CreateDeviceIndependentResources() {
  D2D1_FACTORY_OPTIONS options;
  ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

  winrt::check_hresult(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                         __uuidof(ID2D1Factory2), &options,
                                         &m_d2dFactory));
}

void DXGraphics::CreateDeviceResources() {
  UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

  D3D_FEATURE_LEVEL featureLevels[] = {
      D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_9_3,  D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1};

  // Create the DX11 API device object, and get a corresponding context.
  ComPtr<ID3D11Device> device;
  ComPtr<ID3D11DeviceContext> context;
  winrt::check_hresult(D3D11CreateDevice(
      nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, creationFlags, featureLevels,
      ARRAYSIZE(featureLevels), D3D11_SDK_VERSION, &device, NULL, &context));

  // Get D3D11.1 device
  winrt::check_hresult(device.As(&m_d3dDevice));

  // Get D3D11.1 context
  winrt::check_hresult(context.As(&m_d3dContext));

  // Get underlying DXGI device of D3D device
  ComPtr<IDXGIDevice> dxgiDevice;
  winrt::check_hresult(m_d3dDevice.As(&dxgiDevice));

  m_loadingComplete = true;
}

void DXGraphics::CreateSizeDependentResources() {
  m_renderTargetWidth = m_width * m_compositionScaleX;
  m_renderTargetHeight = m_height * m_compositionScaleY;

  // // If the swap chain already exists, then resize it.
  //if (m_swapChain != nullptr) {
  //  HRESULT hr = m_swapChain->ResizeBuffers(
  //      2, static_cast<UINT>(m_renderTargetWidth),
  //      static_cast<UINT>(m_renderTargetHeight), DXGI_FORMAT_B8G8R8A8_UNORM, 0);

  //  if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET) {
  //    OnDeviceLost();
  //    return;

  //  } else {
  //    winrt::check_hresult(hr);
  //  }
  //} else  // Otherwise, create a new one.
  //{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
    swapChainDesc.Width = static_cast<UINT>(m_renderTargetWidth);
    swapChainDesc.Height = static_cast<UINT>(m_renderTargetHeight);
    swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapChainDesc.Stereo = false;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage =
        DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = 2;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    swapChainDesc.Flags = 0;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

    // Get underlying DXGI Device from D3D Device.
    ComPtr<IDXGIDevice1> dxgiDevice;
    winrt::check_hresult(m_d3dDevice.As(&dxgiDevice));

    // Get adapter.
    ComPtr<IDXGIAdapter> dxgiAdapter;
    winrt::check_hresult(dxgiDevice->GetAdapter(&dxgiAdapter));

    // Get factory.
    ComPtr<IDXGIFactory2> dxgiFactory;
    winrt::check_hresult(dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory)));

    ComPtr<IDXGISwapChain1> swapChain;
    // Create swap chain.
    winrt::check_hresult(dxgiFactory->CreateSwapChainForComposition(
        m_d3dDevice.Get(), &swapChainDesc, nullptr, &swapChain));

    swapChain.As(&m_swapChain);

    winrt::check_hresult(dxgiDevice->SetMaximumFrameLatency(1));

    Dispatcher->RunAsync(
        CoreDispatcherPriority::Normal,
        ref new DispatchedHandler(
            [=]() {
              // Get backing native interface for SwapChainPanel.
              ComPtr<ISwapChainPanelNative> panelNative;
              winrt::check_hresult(
                  reinterpret_cast<IUnknown*>(this)->QueryInterface(
                      IID_PPV_ARGS(&panelNative)));

              winrt::check_hresult(
                  panelNative->SetSwapChain(m_swapChain.Get()));
            },
            CallbackContext::Any));
  //}
}

DXGraphics::~DXGraphics() {  }

void DXGraphics::OnDeviceLost() {
  m_loadingComplete = false;

  m_swapChain = nullptr;

  // Make sure the rendering state has been released.
  m_d3dContext->OMSetRenderTargets(0, nullptr, nullptr);

  m_d2dDevice = nullptr;

  m_d3dContext->Flush();

  CreateDeviceResources();
  CreateSizeDependentResources();

}

void DXGraphics::OnSizeChanged(Object ^ sender, SizeChangedEventArgs ^ e) {
  if (m_width != e->NewSize.Width || m_height != e->NewSize.Height) {
    critical_section::scoped_lock lock(m_criticalSection);

    m_width = max(e->NewSize.Width, 1.0f);
    m_height = max(e->NewSize.Height, 1.0f);

    CreateSizeDependentResources();
  }
}

void DXGraphics::OnCompositionScaleChanged(SwapChainPanel ^ sender,
                                                Object ^ args) {
  if (m_compositionScaleX != CompositionScaleX ||
      m_compositionScaleY != CompositionScaleY) {
    critical_section::scoped_lock lock(m_criticalSection);

    m_compositionScaleX = this->CompositionScaleX;
    m_compositionScaleY = this->CompositionScaleY;

    CreateSizeDependentResources();
  }
}

void DXGraphics::PlayPauseVideo() { m_videoPlayer->PlayPauseVideo(); }

void DXGraphics::OpenURL(Platform::String ^ sURL) {
  const WCHAR* url = sURL->Data();

  m_videoPlayer->OpenURL(url);
}

void DXGraphics::SetPosition(Windows::Foundation::TimeSpan position) {
  m_videoPlayer->SetPosition(position.Duration);
}

long long DXGraphics::GetDuration() {
  return m_videoPlayer->GetDuration();
}
