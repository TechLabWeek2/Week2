#include "FShaderResource.h"
#include "d3dcompiler.h"
#include "FGraphicsDevice.h"

FShaderResource DefaultShader;
FShaderResource CheckerShader;

void FShaderResource::CreateShaderResource()
{
	ID3DBlob* vertexshaderCSO;
	ID3DBlob* pixelshaderCSO;

	HRESULT hr = D3DCompileFromFile(*VertexShaderName, nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);

	if (FAILED(hr))
	{
		assert(false);
	}

	hr = GGraphicsDevice.GetDevice()->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &VertexShader);

	if (FAILED(hr))
	{
		assert(false);
	}

	hr = D3DCompileFromFile(*PixelShaderName, nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);
	//hr = D3DCompileFromFile(L"CheckPattern.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);
	if (FAILED(hr))
	{
		assert(false);
	}

	hr = GGraphicsDevice.GetDevice()->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &PixelShader);
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

	//hr = GGraphicsDevice.GetDevice()->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &InputLayout);
	hr = GGraphicsDevice.GetDevice()->CreateInputLayout(InputLayoutDESC.Data(), InputLayoutDESC.Num(), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &InputLayout);
	if (FAILED(hr))
	{
		assert(false);
	}

	vertexshaderCSO->Release();
	pixelshaderCSO->Release();
}

ID3D11VertexShader* FShaderResource::GetVertexShader() const
{
	return VertexShader;
}

ID3D11PixelShader* FShaderResource::GetPixelShader() const
{
	return PixelShader;
}

ID3D11InputLayout* FShaderResource::GetInputLayout() const
{
	return InputLayout;
}

const FString& FShaderResource::GetVertexShaderName() const
{
	return VertexShaderName;
}

void FShaderResource::SetVertexShaderName(const FString& Name)
{
	VertexShaderName = Name;
}

const FString& FShaderResource::GetPixelShaderName() const
{
	return PixelShaderName;
}

void FShaderResource::SetPixelShaderName(const FString& Name)
{
	PixelShaderName = Name;
}

const D3D11_INPUT_ELEMENT_DESC* FShaderResource::GetInputLayoutDESC()const 
{
	return InputLayoutDESC.Data();
}

void FShaderResource::SetInputLayoutDesc(TArray<D3D11_INPUT_ELEMENT_DESC>& DESC)
{
	InputLayoutDESC = DESC;
}
