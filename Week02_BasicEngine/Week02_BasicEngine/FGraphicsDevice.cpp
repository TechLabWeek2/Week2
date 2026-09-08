#include "FGraphicsDevice.h"
#include <assert.h>

FGraphicsDevice GGraphicsDevice;
ExampleAppConsole Console;

void FGraphicsDevice::Initialize(HWND hWindow, UINT screenWidth, UINT screenHeight)
{
	//Device, DeviceContext, ViewportInfo, SwapChain
	CreateDeviceAndSwapChain(hWindow);

	//SwapChain용 RenderTarget, View
	CreateFrameBuffer();

	//Depth Stencil Buffer, View
	CreateDepthStencilBuffer(screenWidth, screenHeight);
}

void FGraphicsDevice::Release()
{
	//Device, DeviceContext, SwapChain
	ReleaseDeviceAndSwapChain();
	//SwapChain용 RenderTarget, View
	ReleaseFrameBuffer();
	//DepthStencilBuffer, View
	ReleaseDepthStencilBuffer();
}

void FGraphicsDevice::CreateDeviceAndSwapChain(HWND hWindow)
{
	// 지원하는 Direct3D 기능 레벨을 정의
	D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

	// 스왑 체인 설정 구조체 초기화
	DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
	swapchaindesc.BufferDesc.Width = 0; // 창 크기에 맞게 자동으로 설정
	swapchaindesc.BufferDesc.Height = 0; // 창 크기에 맞게 자동으로 설정
	swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // 색상 포맷
	swapchaindesc.SampleDesc.Count = 1; // 멀티 샘플링 비활성화
	swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 렌더 타겟으로 사용
	swapchaindesc.BufferCount = 2; // 더블 버퍼링
	swapchaindesc.OutputWindow = hWindow; // 렌더링할 창 핸들
	swapchaindesc.Windowed = TRUE; // 창 모드
	swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 스왑 방식

	// Direct3D 장치와 스왑 체인을 생성
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
								  D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
								  featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
								  &swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);

	if (FAILED(hr))
	{
		assert(false);
	}
	// 생성된 스왑 체인의 정보 가져오기
	SwapChain->GetDesc(&swapchaindesc);

	// 뷰포트 정보 설정	
	ViewportInfo = { 0.0f, 0.0f, (float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height, 0.0f, 1.0f };
}

void FGraphicsDevice::ReleaseDeviceAndSwapChain()
{
	if (Device)
	{
		Device->Release();
		Device = nullptr;
	}

	if (DeviceContext)
	{
		DeviceContext->Release();
		DeviceContext = nullptr;
	}
	if (SwapChain)
	{
		SwapChain->Release();
		SwapChain = nullptr;
	}
}

void FGraphicsDevice::CreateFrameBuffer()
{
	assert(SwapChain != nullptr);	

	// 스왑 체인으로부터 백 버퍼 텍스처 가져오기
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

	// 렌더 타겟 뷰 생성
	D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
	framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // 색상 포맷
	framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처

	HRESULT hr = Device->CreateRenderTargetView(FrameBuffer, &framebufferRTVdesc, &FrameBufferRTV);
	if (FAILED(hr))
	{
		assert(false);
	}
}

void FGraphicsDevice::ReleaseFrameBuffer()
{
	if (FrameBuffer)
	{
		FrameBuffer->Release();
		FrameBuffer = nullptr;
	}
	if (FrameBufferRTV)
	{
		FrameBufferRTV->Release();
		FrameBufferRTV = nullptr;
	}
}

void FGraphicsDevice::SwapBuffer()
{
	assert(SwapChain != nullptr);
	if (SwapChain == nullptr) return;

	SwapChain->Present(1, 0); // 1: VSync 활성화
}

void FGraphicsDevice::CreateDepthStencilBuffer(UINT screenWidth, UINT screenHeight)
{
	D3D11_TEXTURE2D_DESC depthstencildesc = {};
	depthstencildesc.Width = screenWidth;
	depthstencildesc.Height = screenHeight;
	depthstencildesc.MipLevels = 1;
	depthstencildesc.ArraySize = 1;
	depthstencildesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthstencildesc.SampleDesc.Count = 1;
	depthstencildesc.Usage = D3D11_USAGE_DEFAULT;
	depthstencildesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	HRESULT hr = Device->CreateTexture2D(&depthstencildesc, nullptr, &DepthStencilBuffer);
	if (FAILED(hr))
	{
		assert(false);
	}
	hr = Device->CreateDepthStencilView(DepthStencilBuffer, nullptr, &DepthStencilView);
	if (FAILED(hr))
	{
		assert(false);
	}
}

void FGraphicsDevice::ReleaseDepthStencilBuffer()
{
	if (DepthStencilBuffer)
	{
		DepthStencilBuffer->Release();
		DepthStencilBuffer = nullptr;
	}
	if (DepthStencilView)
	{
		DepthStencilView->Release();
		DepthStencilView = nullptr;
	}
}

ID3D11Device* FGraphicsDevice::GetDevice() const
{
	return Device;
}

ID3D11DeviceContext* FGraphicsDevice::GetDeviceContext() const
{
	return DeviceContext;
}

IDXGISwapChain* FGraphicsDevice::GetSwapChain() const
{
	return SwapChain;
}

D3D11_VIEWPORT FGraphicsDevice::GetViewport() const
{
	return ViewportInfo;
}

void FGraphicsDevice::ResizeViewport(UINT width, UINT height)
{
	ViewportInfo.Width = (float)width;
	ViewportInfo.Height = (float)height;
}

void FGraphicsDevice::BindViewport(ID3D11DeviceContext* context)
{
	context->RSSetViewports(1, &ViewportInfo);
}

ID3D11Texture2D* FGraphicsDevice::GetRenderTarget() const
{
	return FrameBuffer;
}
void FGraphicsDevice::SetRenderTarget(ID3D11Texture2D* frameBuffer)
{
	FrameBuffer = frameBuffer;
}

ID3D11RenderTargetView* FGraphicsDevice::GetRenderTargetView() const
{
	return FrameBufferRTV;
}

void FGraphicsDevice::SetRenderTargetView(ID3D11RenderTargetView* rtv)
{
	FrameBufferRTV = rtv;
}


ID3D11Texture2D* FGraphicsDevice::GetDepthStencilBuffer() const
{
	return DepthStencilBuffer;
}
void FGraphicsDevice::SetDepthStencilBuffer(ID3D11Texture2D* dsb)
{
	DepthStencilBuffer = dsb;
}

ID3D11DepthStencilView* FGraphicsDevice::GetDepthStencilView() const
{
	return DepthStencilView;
}
void FGraphicsDevice::SetDepthStencilView(ID3D11DepthStencilView* dsv)
{
	DepthStencilView = dsv;
}
