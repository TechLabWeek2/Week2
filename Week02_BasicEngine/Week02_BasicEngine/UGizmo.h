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
    static UClass* StaticClass()
    {
        static UClass Class("USphere", UPrimitiveComponent::StaticClass());
        return &Class;
    }
};