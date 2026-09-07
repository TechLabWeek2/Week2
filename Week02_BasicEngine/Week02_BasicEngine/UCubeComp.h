#pragma once
#include "UPrimitiveComponent.h"

class ID3D11Buffer;
class ID3D11Buffer;

class UCubeComp :
    public UPrimitiveComponent
{
public:
    ID3D11Buffer* Vertices;
    int32 NumVertices;

public:
    UCubeComp();

};

