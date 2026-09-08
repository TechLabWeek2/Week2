#pragma once
#include "USceneComponent.h"
#include "URenderer.h"

enum class ETypePrimitive : uint8
{
	None,
	Plane,
	Cube,
	Sphere,
	Floor,
	XLine,
	YLine,
	ZLine,
	Gizmo,
	Max
};


class FMeshResource;
class URenderer;

class UPrimitiveComponent :
    public USceneComponent
{
    using Super = USceneComponent;

public:
    ETypePrimitive primitiveType;

	ID3D11Buffer* Vertices;
	int32 NumVertices;

    UPrimitiveComponent();

    void Update(float deltaTime) override;

    FMeshResource* GetMeshResource() const;
    void SetMeshResource(FMeshResource* pMeshResource);

	static UClass* StaticClass() {
		static UClass Class("UPrimitiveComponent", USceneComponent::StaticClass());
		return &Class;
	}
private:
    //메시 데이터
    FMeshResource* MeshResource = nullptr;
};