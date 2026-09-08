#pragma once
#include "FMeshResource.h"
class FSphereResource :
    public FMeshResource
{
    using Super = FMeshResource;
public:
    void Initialize()override;
};

