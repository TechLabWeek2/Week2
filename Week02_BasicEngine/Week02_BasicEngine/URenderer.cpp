#include "URenderer.h"
#include "UCameraComp.h"
#include "FGraphicsDevice.h"
#include "Shapes.h"
#include "UPrimitiveComponent.h"
#include "FMeshResource.h"
#include "d3dcompiler.h"


void URenderer::CreateConstantBuffer() {
	D3D11_BUFFER_DESC constantbufferdesc = {};

	//Fconstants 64바이트이므로
	constantbufferdesc.ByteWidth = sizeof(FConstants);

	constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC; // will be updated from CPU every frame
	constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	HRESULT hr = Device->CreateBuffer(&constantbufferdesc, nullptr, &ConstantBuffer);
	if (FAILED(hr))
	{
		assert(false);
	}
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
void URenderer::UpdateConstant(FConstants& ConstantData)
{
	if (ConstantBuffer)
	{
		D3D11_MAPPED_SUBRESOURCE constantbufferMSR;

		DeviceContext->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant buffer every frame

		FConstants* constants = (FConstants*)constantbufferMSR.pData;
		{
			constants->MVP = ConstantData.MVP;

			// HighLightIntensity를 bIsSelected 값에 따라 설정
			constants->HightLightIntensity = ConstantData.HightLightIntensity; 
			constants->UseColor = ConstantData.UseColor;

			constants->Color[0] = ConstantData.Color[0];
			constants->Color[1] = ConstantData.Color[1];
			constants->Color[2] = ConstantData.Color[2];
			constants->Color[3] = ConstantData.Color[3];
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

	HRESULT hr = D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);

	if (FAILED(hr))
	{
		assert(false);
	}

	hr = Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &SimpleVertexShader);

	if (FAILED(hr))
	{
		assert(false);
	}

	hr = D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);
	if (FAILED(hr))
	{
		assert(false);
	}

	hr = Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &SimplePixelShader);
	if (FAILED(hr))
	{
		assert(false);
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &SimpleInputLayout);
	if (FAILED(hr))
	{
		assert(false);
	}

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

// 래스터라이저 상태를 생성하는 함수
void URenderer::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC rasterizerdesc = {};
	rasterizerdesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드
	rasterizerdesc.CullMode = D3D11_CULL_BACK; // 백 페이스 컬링

	HRESULT hr = Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState_Solid);
	if (FAILED(hr))
	{
		assert(false);
	}

	rasterizerdesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerdesc.CullMode = D3D11_CULL_BACK; // 백 페이스 컬링
	hr = Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState_WireFrame);
	if (FAILED(hr))
	{
		assert(false);
	}

	rasterizerdesc.FillMode = D3D11_FILL_SOLID;
	rasterizerdesc.CullMode = D3D11_CULL_FRONT; // 백 페이스 컬링
	hr = Device->CreateRasterizerState(&rasterizerdesc, &RasterizerState_FrontCulling);
	if (FAILED(hr))
	{
		assert(false);
	}
}

// 래스터라이저 상태를 해제하는 함수
void URenderer::ReleaseRasterizerState()
{
	if (RasterizerState_Solid)
	{
		RasterizerState_Solid->Release();
		RasterizerState_Solid = nullptr;
	}
}

// 렌더러에 사용된 모든 리소스를 해제하는 함수
void URenderer::Release()
{
	RasterizerState_Solid->Release();

	// 렌더 타겟을 초기화
	DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);

	ReleaseDepthStencilState();
}

//D3D11 렌더링에 필요한 준비 작업을 위한 Prepare 함수
void URenderer::Prepare()
{
	DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);

	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	DeviceContext->RSSetViewports(1, &ViewportInfo);
	//DeviceContext->RSSetState(RasterizerState_Solid);

	DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, DepthStencilView);
	//DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
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

void URenderer::CreateDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC depthstencildesc = {};
	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencildesc.StencilEnable = false;

	HRESULT hr = Device->CreateDepthStencilState(&depthstencildesc, &DepthStencilState);
	if (FAILED(hr))
	{
		assert(false);
	}
}

void URenderer::ReleaseDepthStencilState()
{
	if (DepthStencilState)
	{
		DepthStencilState->Release();
	}
}

void URenderer::RenderScene(const TArray<UObject*> Objects, const UCameraComp* Camera)
{
	if (Objects.IsEmpty()) return;

	/*for (UObject* Obj : GUObjectArray.GetAllObjects())
	{

	}*/

	for (int i = 0; i < Objects.Num(); i++)
	{
		if (Objects[i] == nullptr) continue;

		//UPrimitiveComponent만 Render하도록
		UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Objects[i]);
		if (PrimitiveComponent && PrimitiveComponent->bIsActive)
		{
			DeviceContext->RSSetState(FindRasterizerState(PrimitiveComponent->GetRasterizerState()));
			DeviceContext->OMSetBlendState(FindBlendState(PrimitiveComponent->GetBlendMode()), nullptr, 0xffffffff);
			FConstants TempConstantData = {};
			TempConstantData.MVP = PrimitiveComponent->GetModelMatrix() * Camera->GetViewMatrix() * Camera->GetProjectionMatrix();
			TempConstantData.HightLightIntensity = PrimitiveComponent->bIsSelected ? 2.0 : 1.0;
			TempConstantData.Color[0] = PrimitiveComponent->GetModelColor()[0];
			TempConstantData.Color[1] = PrimitiveComponent->GetModelColor()[1];
			TempConstantData.Color[2] = PrimitiveComponent->GetModelColor()[2];
			TempConstantData.Color[3] = PrimitiveComponent->GetModelColor()[3];
			TempConstantData.UseColor = PrimitiveComponent->GetUseColorFlag();
			UpdateConstant(TempConstantData);
			//UpdateConstant(PrimitiveComponent->GetModelMatrix() * Camera->GetViewMatrix() * Camera->GetProjectionMatrix(), PrimitiveComponent->bIsSelected);
			DeviceContext->IASetPrimitiveTopology(PrimitiveComponent->GetMeshResource()->GetTopology());
			RenderPrimitive(PrimitiveComponent->GetMeshResource()->GetVertexBuffer(), PrimitiveComponent->GetMeshResource()->GetNumVertices());
		}
	}
	 
}

void URenderer::Init()
{
	Device = GGraphicsDevice.GetDevice();
	DeviceContext = GGraphicsDevice.GetDeviceContext();
	SwapChain = GGraphicsDevice.GetSwapChain();
	ViewportInfo = GGraphicsDevice.GetViewport();

	FrameBuffer = GGraphicsDevice.GetRenderTarget();
	FrameBufferRTV = GGraphicsDevice.GetRenderTargetView();

	CreateRasterizerState();

	DepthStencilBuffer = GGraphicsDevice.GetDepthStencilBuffer();
	DepthStencilView = GGraphicsDevice.GetDepthStencilView();

	CreateDepthStencilState();
	
	CreateBlendState();
}

ID3D11RasterizerState* URenderer::FindRasterizerState(RasterizerState StateType)const 
{
	switch (StateType)
	{
		case Solid:
			return RasterizerState_Solid;
			break;
		case WireFrame:
			return RasterizerState_WireFrame;
			break;
		case FrontCulling:
			return RasterizerState_FrontCulling;
			break;
		default:
			break;
	}
	return nullptr;
}

void URenderer::ResizeWindow(bool* bStopRender)
{
	
}

void URenderer::ReleaseWindow()
{
	if (FrameBufferRTV)
	{
		FrameBufferRTV->Release();
		FrameBufferRTV = nullptr;
	}
	//if (Depth)
}

void URenderer::RecreateWindow()
{

}

void URenderer::CreateBlendState()
{
	assert(Device);
	if (!Device) return;

	//불투명
	D3D11_BLEND_DESC OpaqueDesc = {};
	OpaqueDesc.RenderTarget[0].BlendEnable = FALSE;
	OpaqueDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HRESULT hr = Device->CreateBlendState(&OpaqueDesc, &BlendState_Opaque);
	if (FAILED(hr))
	{
		assert(false);
	}

	//반투명
	D3D11_BLEND_DESC AlphaDesc = {};
	AlphaDesc.RenderTarget[0].BlendEnable = TRUE;
	AlphaDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	AlphaDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;  // 기존 화면색 * (1-알파)
	AlphaDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;         // 둘을 더함
	AlphaDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	AlphaDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	AlphaDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;	
	AlphaDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = Device->CreateBlendState(&AlphaDesc, &BlendState_Alpha);
	if (FAILED(hr))
	{
		assert(false);  
	}
}

ID3D11BlendState* URenderer::FindBlendState(BlendMode BlendStateMode) const
{
	switch (BlendStateMode)
	{
		case Opaque:
			return BlendState_Opaque;
			break;
		case Alpha:
			return BlendState_Alpha;
			break;
		default:
			break;
	}
	return nullptr;
}
