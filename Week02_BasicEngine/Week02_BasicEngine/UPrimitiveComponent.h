#pragma once
#include "USceneComponent.h"
#include "URenderer.h"

enum class ETypePrimitive : uint8
{
	None,
	Triangle,
	Cube,
	Sphere,
	XLine,
	YLine,
	ZLine,
	Max
};

class UPrimitiveComponent :
    public USceneComponent
{
public:
    ETypePrimitive primitiveType;

	ID3D11Buffer* Vertices;
	int32 NumVertices;

    UPrimitiveComponent();

    virtual void Render(URenderer* Renderer) = 0;
};