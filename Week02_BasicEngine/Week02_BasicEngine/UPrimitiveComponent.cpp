#include "UPrimitiveComponent.h"

UPrimitiveComponent::UPrimitiveComponent()
	: primitiveType(EPT_None)
{
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
