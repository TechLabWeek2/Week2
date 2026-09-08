#pragma once
#include "FMeshResource.h"
class FPlaneResource :
    public FMeshResource
{
    using Super = FMeshResource;
public:
    void Initialize()override;
};

