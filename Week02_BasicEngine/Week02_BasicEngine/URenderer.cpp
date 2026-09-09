#include "URenderer.h"
#include "UCameraComp.h"
#include "FGraphicsDevice.h"
#include "Shapes.h"
#include "UPrimitiveComponent.h"
#include "FMeshResource.h"
#include "d3dcompiler.h"
#include "FShaderResource.h"
#include "UGizmo.h"

void URenderer::CreateConstantBuffer() {
	D3D11_BUFFER_DESC constantbufferdesc = {};

	//Fconstants 64바이트이므로
	constantbufferdesc.ByteWidth = sizeof(FConstants);

	constantbufferdesc.Usage = D3D11_USAGE_DYNAMIC; // will be updated from CPU every frame
	constantbufferdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantbufferdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	HRESULT hr = GGraphicsDevice.GetDevice()->CreateBuffer(&constantbufferdesc, nullptr, &ConstantBuffer);
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

		GGraphicsDevice.GetDeviceContext()->Map(ConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantbufferMSR); // update constant buffer every frame

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

			constants->PatternNum = ConstantData.PatternNum;
		}

		GGraphicsDevice.GetDeviceContext()->Unmap(ConstantBuffer, 0);
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

	hr = GGraphicsDevice.GetDevice()->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &SimpleVertexShader);

	if (FAILED(hr))
	{
		assert(false);
	}

	hr = D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);
	//hr = D3DCompileFromFile(L"CheckPattern.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);
	if (FAILED(hr))
	{
		assert(false);
	}

	hr = GGraphicsDevice.GetDevice()->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &SimplePixelShader);
	if (FAILED(hr))
	{
		assert(false); 
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = GGraphicsDevice.GetDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &SimpleInputLayout);
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

	HRESULT hr = GGraphicsDevice.GetDevice()->CreateRasterizerState(&rasterizerdesc, &RasterizerState_Solid);
	if (FAILED(hr))
	{
		assert(false);
	}

	rasterizerdesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerdesc.CullMode = D3D11_CULL_BACK; // 백 페이스 컬링
	hr = GGraphicsDevice.GetDevice()->CreateRasterizerState(&rasterizerdesc, &RasterizerState_WireFrame);
	if (FAILED(hr))
	{
		assert(false);
	}

	rasterizerdesc.FillMode = D3D11_FILL_SOLID;
	rasterizerdesc.CullMode = D3D11_CULL_FRONT; // 백 페이스 컬링
	hr = GGraphicsDevice.GetDevice()->CreateRasterizerState(&rasterizerdesc, &RasterizerState_FrontCulling);
	if (FAILED(hr))
	{
		assert(false);
	}

	rasterizerdesc.FillMode = D3D11_FILL_SOLID;
	rasterizerdesc.CullMode = D3D11_CULL_NONE; // 백 페이스 컬링
	hr = GGraphicsDevice.GetDevice()->CreateRasterizerState(&rasterizerdesc, &RasterizerState_Solid_CullingNone);
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
	GGraphicsDevice.GetDeviceContext()->OMSetRenderTargets(0, nullptr, nullptr);

	ReleaseDepthStencilState();

	ReleaseBlendState();
}

//D3D11 렌더링에 필요한 준비 작업을 위한 Prepare 함수
void URenderer::Prepare()
{
	GGraphicsDevice.GetDeviceContext()->ClearRenderTargetView(GGraphicsDevice.GetRenderTargetView(), ClearColor);

	GGraphicsDevice.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	GGraphicsDevice.BindViewport(GGraphicsDevice.GetDeviceContext());
	//GGraphicsDevice.GetDeviceContext()->RSSetState(RasterizerState_Solid);

	ID3D11RenderTargetView* tempRTV = GGraphicsDevice.GetRenderTargetView();
	GGraphicsDevice.GetDeviceContext()->OMSetRenderTargets(1, &tempRTV, GGraphicsDevice.GetDepthStencilView());
	//GGraphicsDevice.GetDeviceContext()->OMSetBlendState(nullptr, nullptr, 0xffffffff);
	GGraphicsDevice.GetDeviceContext()->OMSetDepthStencilState(DepthStencilState, 1);
	GGraphicsDevice.GetDeviceContext()->ClearDepthStencilView(GGraphicsDevice.GetDepthStencilView(), D3D11_CLEAR_DEPTH, 1.f, 0);
}

//Simple Shader 사용을 위한 PrepareShader 함수
void URenderer::PrepareShader()
{
	GGraphicsDevice.GetDeviceContext()->VSSetShader(SimpleVertexShader, nullptr, 0);
	GGraphicsDevice.GetDeviceContext()->PSSetShader(SimplePixelShader, nullptr, 0);
	GGraphicsDevice.GetDeviceContext()->IASetInputLayout(SimpleInputLayout);

	//버텍스 쉐이더에 상수 버퍼 설정
	if (ConstantBuffer) {
		GGraphicsDevice.GetDeviceContext()->VSSetConstantBuffers(0, 1, &ConstantBuffer);
	}
}

//실질적인 Rendering 요청을 할 RenderPrimitive 함수
void URenderer::RenderPrimitive(ID3D11Buffer* pBuffer, UINT numVertices)
{
	UINT offset = 0;
	GGraphicsDevice.GetDeviceContext()->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &offset);

	GGraphicsDevice.GetDeviceContext()->Draw(numVertices, 0);
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

	GGraphicsDevice.GetDevice()->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &vertexBuffer);

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

	HRESULT hr = GGraphicsDevice.GetDevice()->CreateDepthStencilState(&depthstencildesc, &DepthStencilState);
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

	TArray<UPrimitiveComponent*> OpaqueList;
	TArray<UPrimitiveComponent*> AlphaList;

	//불투명과 반투명으로 나눔
	for (UObject* Obj : Objects)
	{
		UPrimitiveComponent* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Obj);
		if (PrimitiveComponent == nullptr) continue;

		if (PrimitiveComponent->GetBlendMode() == BlendMode::Opaque)
		{
			OpaqueList.Add(PrimitiveComponent);
		}
		else
		{
			AlphaList.Add(PrimitiveComponent);
		}
	}

	//불투명 그리기
	for (UPrimitiveComponent* Obj : OpaqueList)
	{
		if (Obj->bIsActive)
		{
			//Shader Set
			GGraphicsDevice.GetDeviceContext()->VSSetShader(Obj->GetShaderResource()->GetVertexShader(), nullptr, 0);
			GGraphicsDevice.GetDeviceContext()->PSSetShader(Obj->GetShaderResource()->GetPixelShader(), nullptr, 0);
			GGraphicsDevice.GetDeviceContext()->IASetInputLayout(Obj->GetShaderResource()->GetInputLayout());  

			//Constant Buffer Set
			GGraphicsDevice.GetDeviceContext()->VSSetConstantBuffers(0, 1, &ConstantBuffer);
			GGraphicsDevice.GetDeviceContext()->PSSetConstantBuffers(0, 1, &ConstantBuffer);

			//Rasterizer State Set
			GGraphicsDevice.GetDeviceContext()->RSSetState(FindRasterizerState(Obj->GetRasterizerState()));
			//BlendState Set
			GGraphicsDevice.GetDeviceContext()->OMSetBlendState(FindBlendState(Obj->GetBlendMode()), nullptr, 0xffffffff);

			//Constant Buffer Update
			FConstants TempConstantData = {};

			TempConstantData.MVP = Obj->GetQuatModelMatrix() * Camera->GetViewMatrix() * Camera->GetProjectionMatrix();
			TempConstantData.HightLightIntensity = Obj->bIsSelected ? 2.0 : 1.0;
			TempConstantData.Color[0] = Obj->GetModelColor()[0];
			TempConstantData.Color[1] = Obj->GetModelColor()[1];
			TempConstantData.Color[2] = Obj->GetModelColor()[2];
			TempConstantData.Color[3] = Obj->GetModelColor()[3];
			TempConstantData.UseColor = Obj->GetUseColorFlag();
			TempConstantData.PatternNum = 2000;
			UpdateConstant(TempConstantData);

			//Topology Set
			GGraphicsDevice.GetDeviceContext()->IASetPrimitiveTopology(Obj->GetMeshResource()->GetTopology());

			//Render
			RenderPrimitive(Obj->GetMeshResource()->GetVertexBuffer(), Obj->GetMeshResource()->GetNumVertices());
		}
	}

	//거리 정렬
	SortTranslucentByDistance(AlphaList, Camera->GetLocation());

	//반투명 렌더
	for (UPrimitiveComponent* Obj : AlphaList)
	{
		if (Obj->bIsActive)
		{
			GGraphicsDevice.GetDeviceContext()->VSSetShader(Obj->GetShaderResource()->GetVertexShader(), nullptr, 0);
			GGraphicsDevice.GetDeviceContext()->PSSetShader(Obj->GetShaderResource()->GetPixelShader(), nullptr, 0);
			GGraphicsDevice.GetDeviceContext()->IASetInputLayout(Obj->GetShaderResource()->GetInputLayout());

			GGraphicsDevice.GetDeviceContext()->VSSetConstantBuffers(0, 1, &ConstantBuffer);
			GGraphicsDevice.GetDeviceContext()->PSSetConstantBuffers(0, 1, &ConstantBuffer);

			GGraphicsDevice.GetDeviceContext()->RSSetState(FindRasterizerState(Obj->GetRasterizerState()));
			GGraphicsDevice.GetDeviceContext()->OMSetBlendState(FindBlendState(Obj->GetBlendMode()), nullptr, 0xffffffff);

			FConstants TempConstantData = {};
			TempConstantData.MVP = Obj->GetQuatModelMatrix() * Camera->GetViewMatrix() * Camera->GetProjectionMatrix();
			TempConstantData.HightLightIntensity = Obj->bIsSelected ? 2.0 : 1.0;
			TempConstantData.Color[0] = Obj->GetModelColor()[0];
			TempConstantData.Color[1] = Obj->GetModelColor()[1];
			TempConstantData.Color[2] = Obj->GetModelColor()[2];
			TempConstantData.Color[3] = Obj->GetModelColor()[3];
			TempConstantData.UseColor = Obj->GetUseColorFlag();
			TempConstantData.PatternNum = 2000;
			UpdateConstant(TempConstantData);

			GGraphicsDevice.GetDeviceContext()->IASetPrimitiveTopology(Obj->GetMeshResource()->GetTopology());
			RenderPrimitive(Obj->GetMeshResource()->GetVertexBuffer(), Obj->GetMeshResource()->GetNumVertices());
		}
	}	 
}

void URenderer::Init()
{
	//Device = GGraphicsDevice.GetDevice();
	//DeviceContext = GGraphicsDevice.GetDeviceContext();
	//SwapChain = GGraphicsDevice.GetSwapChain();
	//ViewportInfo = GGraphicsDevice.GetViewport();

	//FrameBuffer = GGraphicsDevice.GetRenderTarget();
	//FrameBufferRTV = GGraphicsDevice.GetRenderTargetView();

	CreateRasterizerState();

	//DepthStencilBuffer = GGraphicsDevice.GetDepthStencilBuffer();
	//DepthStencilView = GGraphicsDevice.GetDepthStencilView();

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
		case Solid_Culling_None:
			return RasterizerState_Solid_CullingNone;
			break;
		default:
			break;
	}
	return nullptr;
}

void URenderer::ResizeWindow(long newScreenWidth, long newScreenHeight, bool* bStopRender)
{
	ReleaseWindow();
	RecreateWindow(newScreenWidth, newScreenHeight);
}

void URenderer::ReleaseWindow()
{
	GGraphicsDevice.GetDeviceContext()->OMSetRenderTargets(0, nullptr, nullptr);
	if (GGraphicsDevice.GetRenderTargetView())
	{
		GGraphicsDevice.GetRenderTargetView()->Release();
		GGraphicsDevice.SetRenderTargetView(nullptr);
	}
	if (GGraphicsDevice.GetDepthStencilView())
	{
		GGraphicsDevice.GetDepthStencilView()->Release();
		GGraphicsDevice.SetDepthStencilView(nullptr);
	}
	if (GGraphicsDevice.GetDepthStencilBuffer())
	{
		GGraphicsDevice.GetDepthStencilBuffer()->Release();
		GGraphicsDevice.SetDepthStencilBuffer(nullptr);
	}
	if (GGraphicsDevice.GetRenderTarget())
	{
		GGraphicsDevice.GetRenderTarget()->Release();
		GGraphicsDevice.SetRenderTarget(nullptr);
	}
}

void URenderer::RecreateWindow(long newScreenWidth, long newScreenHeight)
{
	HRESULT hr = GGraphicsDevice.GetSwapChain()->ResizeBuffers(2, newScreenWidth, newScreenHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
	if (FAILED(hr))
	{
		assert(false);
	}

	ID3D11Texture2D* newBuffer = nullptr;
	HRESULT hr_ = GGraphicsDevice.GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(&newBuffer));
	if (FAILED(hr_))
	{
		assert(false);
	}
	GGraphicsDevice.SetRenderTarget(newBuffer);

	ID3D11RenderTargetView* newRTV;
	D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
	framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
	framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	GGraphicsDevice.GetDevice()->CreateRenderTargetView(newBuffer, &framebufferRTVdesc, &newRTV);
	GGraphicsDevice.SetRenderTargetView(newRTV);

	GGraphicsDevice.CreateDepthStencilBuffer(newScreenWidth, newScreenHeight);
	GGraphicsDevice.ResizeViewport(newScreenWidth, newScreenHeight);
	GGraphicsDevice.BindViewport(GGraphicsDevice.GetDeviceContext());
}


void URenderer::CreateBlendState()
{
	assert(GGraphicsDevice.GetDevice());
	if (!GGraphicsDevice.GetDevice()) return;

	//불투명
	D3D11_BLEND_DESC OpaqueDesc = {};
	OpaqueDesc.RenderTarget[0].BlendEnable = FALSE;
	OpaqueDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	HRESULT hr = GGraphicsDevice.GetDevice()->CreateBlendState(&OpaqueDesc, &BlendState_Opaque);
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

	hr = GGraphicsDevice.GetDevice()->CreateBlendState(&AlphaDesc, &BlendState_Alpha);
	if (FAILED(hr))
	{
		assert(false);  
	}
}

void URenderer::ReleaseBlendState()
{
	if (BlendState_Opaque)
	{
		BlendState_Opaque->Release();
		BlendState_Opaque = nullptr;
	}

	if (BlendState_Alpha)
	{
		BlendState_Alpha->Release();
		BlendState_Alpha = nullptr;
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

void URenderer::SortTranslucentByDistance(TArray<UPrimitiveComponent*>& AlphaList, const FVector& CameraLoc)
{
	std::sort(AlphaList.begin(), AlphaList.end(),
			  [&CameraLoc](UPrimitiveComponent* A, UPrimitiveComponent* B)
			  {
				  float DisA = (A->GetLocation() - CameraLoc).SizeSquared();
				  float DisB = (B->GetLocation() - CameraLoc).SizeSquared();
				  return DisA > DisB;
			  });
}
