#pragma once
#include <windows.h>
#include <d3d11.h>
#include "Core/Core.h"
#include "ExampleAppConsole.h"

class ID3D11Device;
class ID3D11DeviceContext;
class IDXGISwapChain;
class D3D11_VIEWPORT;

class FGraphicsDevice
{
public:
	//초기화
	void Initialize(HWND hWindow, UINT screenWidth, UINT screenHeight);
	//해제
	void Release();

	// Direct3D 장치 및 스왑 체인을 생성하는 함수
	void CreateDeviceAndSwapChain(HWND hWindow);

	// Direct3D 장치 및 스왑 체인을 해제하는 함수
	void ReleaseDeviceAndSwapChain();

	// 프레임 버퍼를 생성하는 함수
	void CreateFrameBuffer();

	// 프레임 버퍼를 해제하는 함수
	void ReleaseFrameBuffer();

	// 스왑 체인의 백 버퍼와 프론트 버퍼를 교체하여 화면에 출력
	void SwapBuffer();

	//DepthStencilBuffer, View 생성
	void CreateDepthStencilBuffer(UINT screenWidth, UINT screenHeight);

	// 깊이 스텐실 버퍼 해제
	void ReleaseDepthStencilBuffer();


	ID3D11Device* GetDevice()const;
	ID3D11DeviceContext* GetDeviceContext()const;
	IDXGISwapChain* GetSwapChain()const;
	D3D11_VIEWPORT GetViewport()const;

	ID3D11Texture2D* GetRenderTarget()const;
	ID3D11RenderTargetView* GetRenderTargetView()const;

	ID3D11Texture2D* GetDepthStencilBuffer() const;
	ID3D11DepthStencilView* GetDepthStencilView() const;

private:
	ID3D11Device* Device = nullptr; // GPU와 통신하기 위한 Direct3D 장치
	ID3D11DeviceContext* DeviceContext = nullptr; // GPU 명령 실행을 담당하는 컨텍스트
	IDXGISwapChain* SwapChain = nullptr; // 프레임 버퍼를 교체하는 데 사용되는 스왑 체인
	D3D11_VIEWPORT ViewportInfo; // 렌더링 영역을 정의하는 뷰포트 정보

	// 렌더링에 필요한 리소스 및 상태를 관리하기 위한 변수들
	ID3D11Texture2D* FrameBuffer = nullptr; // 화면 출력용 텍스처
	ID3D11RenderTargetView* FrameBufferRTV = nullptr; // 텍스처를 렌더 타겟으로 사용하는 뷰

	ID3D11Texture2D* DepthStencilBuffer = nullptr; // 깊이, 스텐실 버퍼
	ID3D11DepthStencilView* DepthStencilView = nullptr; // 깊이 버퍼 연결 인터페이스
};

extern FGraphicsDevice GGraphicsDevice;
extern ExampleAppConsole Console;