#pragma once
#include "USceneComponent.h"
class UCameraComp :
    public USceneComponent
{
public:
    float FOV = 60.0f;
    float AspectRatio = 10.f / 10.f;
    float NearClip = 0.1f;
    float FarClip = 1000.0f;
    FVector ZAxis;
    FVector XAxis;
    FVector YAxis;
public:
    UCameraComp();
    ~UCameraComp();

    FMatrix GetViewMatrix() const;
    FMatrix GetProjectionMatrix() const;
};