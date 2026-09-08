#pragma once
#include "UPrimitiveComponent.h"

enum UTypeTransform {
    Location,
    Rotation,
    Scale,
    Max
};

class UGizmo :
    public UPrimitiveComponent
{
public:
    UTypeTransform Type;
public:
    UGizmo();

};