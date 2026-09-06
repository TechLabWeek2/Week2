#pragma once
#include "UPrimitiveComponent.h"

class UAxisGizmo:
    public UPrimitiveComponent
{
public:
    ID3D11Buffer* Vertices;
    int32 NumVertices;

public:
    UAxisGizmo();

    virtual void Render(URenderer* Renderer) override;
};