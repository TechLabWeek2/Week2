#pragma once
#include "UPrimitiveComponent.h"

enum ETypeTransform {
    Location,
    Rotation,
    Scale
};

enum ETypeAxis {
    XAxis,
    YAxis,
    ZAxis
};

class UGizmo :
    public UPrimitiveComponent
{
public:
    ETypeTransform Type;
    ETypeAxis Axis;
public:
    UGizmo(ETypeAxis axis);
    static UClass* StaticClass()
    {
        static UClass Class("USphere", UPrimitiveComponent::StaticClass());
        return &Class;
    }

    void Update(UPrimitiveComponent* Obj);

    void ObjUpdate(UPrimitiveComponent* Obj, FVector MouseMove);
};