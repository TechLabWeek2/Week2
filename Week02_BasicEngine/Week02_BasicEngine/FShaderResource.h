#pragma once
#include "Core/Core.h"
#include <d3d11.h>

class ID3D11VertexShader;
class ID3D11PixelShader;
class ID3D11InputLayout;

class FShaderResource
{
public:
	void CreateShaderResource();

	ID3D11VertexShader* GetVertexShader() const;
	ID3D11PixelShader* GetPixelShader() const;
	ID3D11InputLayout* GetInputLayout() const;

	const FString& GetVertexShaderName()const;
	void SetVertexShaderName(const FString& Name);

	const FString& GetPixelShaderName()const;
	void SetPixelShaderName(const FString& Name);

	const D3D11_INPUT_ELEMENT_DESC* GetInputLayoutDESC() const;
	void SetInputLayoutDesc(TArray<D3D11_INPUT_ELEMENT_DESC>& DESC);

private:
	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* InputLayout;

	FString VertexShaderName;
	FString PixelShaderName;

	TArray<D3D11_INPUT_ELEMENT_DESC> InputLayoutDESC;
};

extern FShaderResource DefaultShader;
extern FShaderResource CheckerShader;

