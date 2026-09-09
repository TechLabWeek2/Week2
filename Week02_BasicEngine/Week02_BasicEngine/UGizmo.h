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

enum ETypeSpace {
    Local,
    World
};

class UGizmo :
    public UPrimitiveComponent
{
public:
    ETypeTransform Type;
    ETypeAxis Axis;
    ETypeSpace Space;

public:
    UGizmo(ETypeAxis axis);
    static UClass* StaticClass()
    {
        static UClass Class("USphere", UPrimitiveComponent::StaticClass());
        return &Class;
    }

    void Update(UPrimitiveComponent* Obj, USceneComponent* Camera);

    void ObjUpdate(UPrimitiveComponent* Obj, FVector MouseMove, POINT CurrentMousePos, POINT LastMousePos, float ScreenWidth, float ScreenHeight, UCameraComp* Camera);
};