#pragma once
#include "UObject.h"
#include "FVector.h"
#include "Shapes.h"

class USceneComponent :
    public UObject
{
public:
    FVector RelativeLocation;
    FVector RelativeRotation;
    FVector RelativeScale3D;
    ETypePrimitive primitiveType;
public:
    USceneComponent();

    FMatrix GetModelMatrix() const;

};

