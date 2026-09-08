#include "UPrimitiveComponent.h"

UPrimitiveComponent::UPrimitiveComponent()
	: primitiveType(ETypePrimitive::None)
{
	Class = StaticClass();
}

void UPrimitiveComponent::Update(float deltaTime)
{
	Super::Update(deltaTime);
	std::wcout << FString(L"PrimitiveComponent Update") << std::endl;
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
