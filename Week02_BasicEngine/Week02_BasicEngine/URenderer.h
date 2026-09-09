#pragma once
#include <windows.h>
#include <math.h>
#include <d3d11.h>

// D3D 사용에 필요한 라이브러리들을 링크합니다.
#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

// D3D 사용에 필요한 헤더파일들을 포함합니다.
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx11.h"
#include "imGui/imgui_impl_win32.h"

#include "Core/Core.h"
#include "FMeshResource.h"


class UObject;
class UCameraComp;
struct FVertexSimple;

class ID3D11Device;
class ID3D11DeviceContext;
class IDXGISwapChain;
class ID3D11Texture2D;
class ID3D11RenderTargetView;
class ID3D11RasterizerState;
class ID3D11Buffer;
class ID3D11DepthStencilView;
class ID3D11DepthStencilState;
class D3D11_VIEWPORT;
class UPrimitiveComponent;

struct alignas(16) FConstants {
    FMatrix MVP;

	float HightLightIntensity = 1.f;
    bool UseColor = false;
    float PatternNum = 10.f;
	float Padding; // 16바이트 정렬을 위해 패딩 추가
    float Color[4] = { 1.f, 1.f, 1.f, 1.f };
};

class URenderer
{
public:
    UCameraComp* MainCamera;
    //UCameraComp* MainCamera = new UCameraComp();

public:
    // Direct3D 11 장치(Device)와 장치 컨텍스트(Device Context) 및 스왑 체인(Swap Chain)을 관리하기 위한 포인터들
    //ID3D11Device* Device = nullptr; // GPU와 통신하기 위한 Direct3D 장치
    //ID3D11DeviceContext* DeviceContext = nullptr; // GPU 명령 실행을 담당하는 컨텍스트
    //IDXGISwapChain* SwapChain = nullptr; // 프레임 버퍼를 교체하는 데 사용되는 스왑 체인

    // 렌더링에 필요한 리소스 및 상태를 관리하기 위한 변수들
    //ID3D11Texture2D* FrameBuffer = nullptr; // 화면 출력용 텍스처
    //ID3D11RenderTargetView* FrameBufferRTV = nullptr; // 텍스처를 렌더 타겟으로 사용하는 뷰
    ID3D11RasterizerState* RasterizerState_Solid = nullptr; // 래스터라이저 상태(컬링, 채우기 모드 등 정의)
    ID3D11RasterizerState* RasterizerState_WireFrame = nullptr; // 와이어프레임 래스터라이저 상태
    ID3D11RasterizerState* RasterizerState_FrontCulling = nullptr; // 래스터라이저 상태
    ID3D11RasterizerState* RasterizerState_Solid_CullingNone = nullptr; // 래스터라이저 상태
    ID3D11Buffer* ConstantBuffer = nullptr; // 쉐이더에 데이터를 전달하기 위한 상수 버퍼
    //ID3D11Texture2D* DepthStencilBuffer = nullptr; // 깊이, 스텐실 버퍼
    //ID3D11DepthStencilView* DepthStencilView = nullptr; // 깊이 버퍼 연결 인터페이스
    ID3D11DepthStencilState* DepthStencilState = nullptr; // 깊이, 스텐실을 어떻게 처리할지 

    FLOAT ClearColor[4] = { 0.53f, 0.81f, 0.92f, 1.0f }; // 화면을 초기화(clear)할 때 사용할 색상 (하늘색, RGBA)
    //D3D11_VIEWPORT ViewportInfo; // 렌더링 영역을 정의하는 뷰포트 정보

    //불투명용, 반투명용 BlendState
    ID3D11BlendState* BlendState_Opaque = nullptr;
    ID3D11BlendState* BlendState_Alpha = nullptr;

    void CreateConstantBuffer();

    void ReleaseConstantBuffer();

    //상수 버퍼를 갱신하는 함수
    void UpdateConstant(FConstants& ConstantData);

    void CreateShader();

    void ReleaseShader();

        // 래스터라이저 상태를 생성하는 함수
    void CreateRasterizerState();

    // 래스터라이저 상태를 해제하는 함수
    void ReleaseRasterizerState();

    // 렌더러에 사용된 모든 리소스를 해제하는 함수
    void Release();

        //D3D11 렌더링에 필요한 준비 작업을 위한 Prepare 함수
    void Prepare();

    //Simple Shader 사용을 위한 PrepareShader 함수
    void PrepareShader();

    //실질적인 Rendering 요청을 할 RenderPrimitive 함수
    void RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices);

    //버텍스 버퍼 생성
    ID3D11Buffer* CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth);

    void ReleaseVertexBuffer(ID3D11Buffer* vertexBuffer);

        // 깊이 스텐실 state 생성
    void CreateDepthStencilState();

        // 깊이 스텐실 버퍼 생성
    void ReleaseDepthStencilState();

    //GUObjectArray 순회하며 render 호출
    void RenderScene(const TArray<UObject*> Objects, const UCameraComp* Camera);

    //초기화
    void Init();

    //enum에 따라 RasterizerState 선택
    ID3D11RasterizerState* FindRasterizerState(RasterizerState StateType)const;

    void ResizeWindow(long newScreenWidth, long newScreenHeight, bool* bStopRender);

    void ReleaseWindow();

    void RecreateWindow(long newScreenWidth, long newScreenHeight);

    //BlendState 만들기
    void CreateBlendState();

    void ReleaseBlendState();

    //enum에 따라 BlendState 선택
    ID3D11BlendState* FindBlendState(BlendMode BlendStateMode)const;

    //반투명 정렬 함수
    void SortTranslucentByDistance(TArray<UPrimitiveComponent*>& AlphaList, const FVector& CameraLoc);

};
