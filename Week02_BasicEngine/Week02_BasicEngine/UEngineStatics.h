#pragma once

#include "Core/Core.h"

class UEngineStatics
{
public:
    static uint32 GenUUID()
    {
        return NextUUID++;
    }

private:
    static uint32 NextUUID;
};