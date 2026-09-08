#pragma once
#include "USceneComponent.h"
#include "URenderer.h"

enum class ETypePrimitive : uint8
{
	None,
	Plane,
	Cube,
	Sphere,
	XLine,
	YLine,
	ZLine,
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

private:
    //메시 데이터
    FMeshResource* MeshResource = nullptr;
};