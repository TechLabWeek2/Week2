#pragma once
#include "FMeshResource.h"
class FCubeResource :
    public FMeshResource
{
    using Super = FMeshResource;
public:

    void Initialize()override;
};

