#include "URenderer.h"


void URenderer::CreateConstantBuffer() {
	D3D11_BUFFER_DESC constantbufferdesc = {};

	//Fconstants 64바이트이므로
	constantbufferdesc.ByteWidth = sizeof(FConstants);

	constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC; // will be updated from CPU every frame
	constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	Device->CreateBuffer(&constantbufferdesc, nullptr, &ConstantBuffer);
}

void URenderer::ReleaseConstantBuffer()
{
	if (ConstantBuffer)
	{
		ConstantBuffer->Release();
		ConstantBuffer = nullptr;
	}
}

//상수 버퍼를 갱신하는 함수
void URenderer::UpdateConstant(FVector Offset, float Scale, FVector Rotation, FVector CameraLocation, FVector CameraRotation)
{
	if (ConstantBuffer)
	{
		D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

		FMatrix4x4 scaleMatrix =
			FMatrix4x4::Scaling(
				Scale,
				Scale,
				Scale
			);

		FMatrix4x4 rotationMatrix =
			FMatrix4x4::Rotation(
				Rotation.x,
				Rotation.y,
				Rotation.z
			);

		FMatrix4x4 translationMatrix =
			FMatrix4x4::Translation(
				Offset.x,
				Offset.y,
				Offset.z
			);

		FMatrix4x4 worldMatrix =
			scaleMatrix *
			rotationMatrix *
			translationMatrix;
		////////////////////////////////////////////////
		FVector ZAxis = CameraRotation;
		ZAxis.Normalize();
		FVector XAxis;
		XAxis.x = ZAxis.z;
		XAxis.y = 0;
		XAxis.z = -ZAxis.x;
		XAxis.Normalize();
		FVector YAxis;
		YAxis.x = ZAxis.y * XAxis.z - ZAxis.z * XAxis.y;
		YAxis.y = ZAxis.z * XAxis.x - ZAxis.x * XAxis.z;
		YAxis.z = ZAxis.x * XAxis.y - ZAxis.y * XAxis.x;
		YAxis.Normalize();
		FMatrix4x4 M;
		M.m[0][0] = XAxis.x;
		M.m[0][1] = YAxis.x;
		M.m[0][2] = ZAxis.x;
		M.m[0][3] = 0.0f;
		M.m[1][0] = XAxis.y;
		M.m[1][1] = YAxis.y;
		M.m[1][2] = ZAxis.y;
		M.m[1][3] = 0.0f;
		M.m[2][0] = XAxis.z;
		M.m[2][1] = YAxis.z;
		M.m[2][2] = ZAxis.z;
		M.m[2][3] = 0.0f;
		M.m[3][0] = -CameraLocation.x * XAxis.x - CameraLocation.y * XAxis.y - CameraLocation.z * XAxis.z;
		M.m[3][1] = -CameraLocation.x * YAxis.x - CameraLocation.y * YAxis.y - CameraLocation.z * YAxis.z;
		M.m[3][2] = -CameraLocation.x * ZAxis.x - CameraLocation.y * ZAxis.y - CameraLocation.z * ZAxis.z;
		M.m[3][3] = 1.0f;

		FMatrix4x4 M2;
		float nearZ = 0.1f; // 원하는 최소 거리
		float farZ = 1000.f; // 원하는 최대 거리
		
		M2.m[0][0] = 1 / tan(0.52);
		M2.m[1][1] = 1 / tan(0.52);
		M2.m[2][2] = (farZ) / (farZ - nearZ);
		M2.m[2][3] = 1;
		M2.m[3][2] = -(farZ * nearZ) / (farZ - nearZ);
		M2.m[3][3] = 0;
		FMatrix4x4 viewMatrix = worldMatrix * M * M2;
		////////////////////////////////////////////////

		DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant buffer every frame
		if (!(CameraRotation.x == 0 && CameraRotation.y == 0 && CameraRotation.z == 0)) {
			FConstants* constants = (FConstants*)constantbufferMSR.pData;
			{
				constants->World = viewMatrix;
			}
		}
		else {
			FConstants* constants = (FConstants*)constantbufferMSR.pData;
			{
				constants->World = worldMatrix;
			}
		}
		DeviceContext->Unmap(ConstantBuffer, 0);
	}
}

ID3D11VertexShader* SimpleVertexShader;
ID3D11PixelShader* SimplePixelShader;
ID3D11InputLayout* SimpleInputLayout;
unsigned int Stride;

void URenderer::CreateShader()
{
	ID3DBlob* vertexshaderCSO;
	ID3DBlob* pixelshaderCSO;

	D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);

	Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &SimpleVertexShader);

	D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);

	Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &SimplePixelShader);

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &SimpleInputLayout);

	Stride = sizeof(FVertexSimple);

	vertexshaderCSO->Release();
	pixelshaderCSO->Release();
}

void URenderer::ReleaseShader()
{
	if (SimpleInputLayout)
	{
		SimpleInputLayout->Release();
		SimpleInputLayout = nullptr;
	}

	if (SimplePixelShader)
	{
		SimplePixelShader->Release();
		SimplePixelShader = nullptr;
	}

	if (SimpleVertexShader)
	{
		SimpleVertexShader->Release();
		SimpleVertexShader = nullptr;
	}
}

// 렌더러 초기화 함수
void URenderer::Create(HWND hWindow, UINT screenWidth, UINT screenHeight)
{
	// Direct3D 장치 및 스왑 체인 생성
	CreateDeviceAndSwapChain(hWindow);

	// 프레임 버퍼 생성
	CreateFrameBuffer();

	// 래스터라이저 상태 생성
	CreateRasterizerState();

	// 깊이 스텐실 버퍼 및 블렌드 상태
	CreateDepthStencilBuffer(screenWidth, screenHeight);
	CreateDepthStencilState();
}

// Direct3D 장치 및 스왑 체인을 생성하는 함수
void URenderer::CreateDeviceAndSwapChain(HWND hWindow)
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
	D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
		D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,
		featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
		&swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);

	// 생성된 스왑 체인의 정보 가져오기
	SwapChain->GetDesc(&swapchaindesc);

	// 뷰포트 정보 설정
	ViewportInfo = { 0.0f, 0.0f, (float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height, 0.0f, 1.0f };
}

// Direct3D 장치 및 스왑 체인을 해제하는 함수
void URenderer::ReleaseDeviceAndSwapChain()
{
	if (DeviceContext)
	{
		DeviceContext->Flush(); // 남아있는 GPU 명령 실행
	}

	if (SwapChain)
	{
		SwapChain->Release();
		SwapChain = nullptr;
	}

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
}

// 프레임 버퍼를 생성하는 함수
void URenderer::CreateFrameBuffer()
{
	// 스왑 체인으로부터 백 버퍼 텍스처 가져오기
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

	// 렌더 타겟 뷰 생성
	D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
	framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB; // 색상 포맷
	framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처

	Device->CreateRenderTargetView(FrameBuffer, &framebufferRTVdesc, &FrameBufferRTV);
}

// 프레임 버퍼를 해제하는 함수
void URenderer::ReleaseFrameBuffer()
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

// 래스터라이저 상태를 생성하는 함수
void URenderer::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerdesc = {};
	rasterizerdesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드
	rasterizerdesc.CullMode = D3D11_CULL_BACK; // 백 페이스 컬링

	Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState);
}

// 래스터라이저 상태를 해제하는 함수
void URenderer::ReleaseRasterizerState()
{
	if (RasterizerState)
	{
		RasterizerState->Release();
		RasterizerState = nullptr;
	}
}

// 렌더러에 사용된 모든 리소스를 해제하는 함수
void URenderer::Release()
{
	RasterizerState->Release();

	// 렌더 타겟을 초기화
	DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	ReleaseFrameBuffer();
	ReleaseDeviceAndSwapChain();
}

// 스왑 체인의 백 버퍼와 프론트 버퍼를 교체하여 화면에 출력
void URenderer::SwapBuffer()
{
	SwapChain->Present(1, 0); // 1: VSync 활성화
}

//D3D11 렌더링에 필요한 준비 작업을 위한 Prepare 함수
void URenderer::Prepare()
{
	DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);

	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DeviceContext->RSSetViewports(1, &ViewportInfo);
	DeviceContext->RSSetState(RasterizerState);

	DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, DepthStencilView);
	DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	DeviceContext->OMSetDepthStencilState(DepthStencilState, 1);
	DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1.f, 0);
}

//Simple Shader 사용을 위한 PrepareShader 함수
void URenderer::PrepareShader()
{
	DeviceContext->VSSetShader(SimpleVertexShader, nullptr, 0);
	DeviceContext->PSSetShader(SimplePixelShader, nullptr, 0);
	DeviceContext->IASetInputLayout(SimpleInputLayout);

	//버텍스 쉐이더에 상수 버퍼 설정
	if (ConstantBuffer) {
		DeviceContext->VSSetConstantBuffers(0, 1, &ConstantBuffer);
	}
}

//실질적인 Rendering 요청을 할 RenderPrimitive 함수
void URenderer::RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices)
{
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);

	DeviceContext->Draw(numVertices, 0);
}

//버텍스 버퍼 생성
ID3D11Buffer* URenderer::CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth) {
	//2. Create a vertex buffer
	D3D11_BUFFER_DESC vertexbufferdesc = {};
	vertexbufferdesc.ByteWidth = byteWidth;
	vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexbufferSRD = { vertices };

	ID3D11Buffer* vertexBuffer;

	Device->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);

	return vertexBuffer;
}

void URenderer::ReleaseVertexBuffer(ID3D11Buffer* vertexBuffer) {
	vertexBuffer->Release();
}

void URenderer::CreateDepthStencilBuffer(UINT screenWidth, UINT screenHeight)
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

	Device->CreateTexture2D(&depthstencildesc, nullptr, &DepthStencilBuffer);
	Device->CreateDepthStencilView(DepthStencilBuffer, nullptr, &DepthStencilView);
}

void URenderer::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC depthstencildesc = {};
	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencildesc.StencilEnable = false;

	Device->CreateDepthStencilState(&depthstencildesc, &DepthStencilState);
}