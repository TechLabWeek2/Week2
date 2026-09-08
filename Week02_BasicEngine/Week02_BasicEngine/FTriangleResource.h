#pragma once
#include "FMeshResource.h"
class FTriangleResource :
    public FMeshResource
{
    using Super = FMeshResource;
public:
    void Initialize()override;
};

