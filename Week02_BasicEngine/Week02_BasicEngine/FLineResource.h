#pragma once
#include "FMeshResource.h"
class FLineResource :
    public FMeshResource
{
    using Super = FMeshResource;
public:
    void Initialize()override;
};

