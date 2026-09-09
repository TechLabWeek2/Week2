#pragma once
#include "USceneComponent.h"

class UCameraComp :
    public USceneComponent
{
public:
    float FOV = 90.0f;
    float AspectRatio = 1024.f / 1024.f;
    float NearClip = 0.1f;
    float FarClip = 1000.0f;
    float ZoomLevel = 1.f;
    bool IsOrthogonal = false;
    FVector ZAxis;
    FVector XAxis;
    FVector YAxis;
public:
    UCameraComp();
    ~UCameraComp();

    FMatrix GetViewMatrix() const;
    FMatrix GetProjectionMatrix() const;
    FMatrix GetProjectionMatrixOrthogonal() const;

    /*const FVector GetForwardVector_UE()const;
    const FVector GetRightVector_UE()const;
    const FVector GetUpVector_UE()const;*/

};