#pragma once
#include "UPrimitiveComponent.h"

class URenderer;
class ID3D11Buffer;

class UAxisGizmo:
    public UPrimitiveComponent
{
public:
    ID3D11Buffer* Vertices;
    int32 NumVertices;

public:
    UAxisGizmo();

};