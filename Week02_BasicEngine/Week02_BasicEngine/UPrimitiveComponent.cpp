#include "UPrimitiveComponent.h"

UPrimitiveComponent::UPrimitiveComponent()
	: primitiveType(ETypePrimitive::None)
{
	Class = StaticClass();
}

void UPrimitiveComponent::Update(float deltaTime)
{
	Super::Update(deltaTime);
}

FMeshResource* UPrimitiveComponent::GetMeshResource() const
{
	return MeshResource;
}

void UPrimitiveComponent::SetMeshResource(FMeshResource* pMeshResource)
{
	MeshResource = pMeshResource;
}

RasterizerState UPrimitiveComponent::GetRasterizerState() const
{
	return RasterizerStateType;
}

void UPrimitiveComponent::SetRasterizerState(RasterizerState StateType)
{
	RasterizerStateType = StateType;
}

BlendMode UPrimitiveComponent::GetBlendMode() const
{
	return BlendModeState;
}

void UPrimitiveComponent::SetBlendMode(BlendMode NewBlendMode)
{
	BlendModeState = NewBlendMode;
}

const float* UPrimitiveComponent::GetModelColor() const
{
	return ModelColor;
}

void UPrimitiveComponent::SetModelColor(const float NewColor[4])
{
	memcpy(ModelColor, NewColor, sizeof(NewColor));
}

void UPrimitiveComponent::SetModelColor(const TArray<float>& NewColor)
{
	ModelColor[0] = NewColor[0];
	ModelColor[1] = NewColor[1];
	ModelColor[2] = NewColor[2];
	ModelColor[3] = NewColor[3];
}

bool UPrimitiveComponent::GetUseColorFlag() const
{
	return UseColor;
}

void UPrimitiveComponent::SetUseColorFlag(bool pUseColorFlag)
{
	UseColor = pUseColorFlag;
}

FShaderResource* UPrimitiveComponent::GetShaderResource() const
{
	return ShaderResource;
}

void UPrimitiveComponent::SetShaderResource(FShaderResource* pShaderResource)
{
	ShaderResource = pShaderResource;
}
