#include "pch.h"

#include "VideoPlayerWrapper.h"

using namespace VideoPlayerWrapper;

using namespace D2D1;
using namespace Platform;
using namespace Concurrency;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::Storage::Streams;
using namespace Windows::System::Threading;


VideoPlayerWrap::VideoPlayerWrap()
    : m_width(1600.0f),
      m_height(900.0f) {

  CreateDeviceIndependentResources();
  CreateDeviceResources();
  CreateSizeDependentResources();

  m_videoPlayer = new VideoPlayer(m_swapChain.Get());
}

void VideoPlayerWrap::CreateDeviceIndependentResources() {
  D2D1_FACTORY_OPTIONS options;
  ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

  winrt::check_hresult(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                         __uuidof(ID2D1Factory2), &options,
                                         &m_d2dFactory));
}

void VideoPlayerWrap::CreateDeviceResources() {
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

void VideoPlayerWrap::CreateSizeDependentResources() {
  DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
  swapChainDesc.Width = static_cast<UINT>(m_width);
  swapChainDesc.Height = static_cast<UINT>(m_height);
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

            winrt::check_hresult(panelNative->SetSwapChain(m_swapChain.Get()));
          },
          CallbackContext::Any));
}

VideoPlayerWrap::~VideoPlayerWrap() {}

void VideoPlayerWrap::OnDeviceLost() {
  m_loadingComplete = false;

  m_swapChain = nullptr;

  m_d3dContext->OMSetRenderTargets(0, nullptr, nullptr);

  m_d2dDevice = nullptr;

  m_d3dContext->Flush();

  CreateDeviceResources();
  CreateSizeDependentResources();

}

void VideoPlayerWrap::PlayPauseVideo() { m_videoPlayer->PlayPauseVideo(); }

void VideoPlayerWrap::OpenURL(Platform::String ^ sURL) {
  const WCHAR* url = sURL->Data();

  m_videoPlayer->OpenURL(url);
}

void VideoPlayerWrap::SetPosition(Windows::Foundation::TimeSpan position) {
  m_videoPlayer->SetPosition(position.Duration);
}

long long VideoPlayerWrap::GetDuration() {
  return m_videoPlayer->GetDuration(); }

void VideoPlayerWrap::ResizeSwapChainPanel(double width, double height) {
  m_videoPlayer->GetDxHelper()->ResizeRenderTarget(width, height);
}
