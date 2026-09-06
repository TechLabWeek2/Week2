#pragma once
#include "UObject.h"
#include "FVector.h"
class USceneComponent :
    public UObject
{
public:
    FVector RelativeLocation;
    FVector RelativeRotation;
    FVector RelativeScale3D;

public:
    USceneComponent();

    FMatrix GetTransformMatrix() const;
};

