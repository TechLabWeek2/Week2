#pragma once
#include "UObject.h"
#include "FVector.h"
#include "FQuat.h"
#include "Shapes.h"

class USceneComponent :
    public UObject
{
public:
    FVector RelativeLocation;
    FVector RelativeRotation;
    FVector RelativeScale3D;
    FQuat RelativeQ;
    bool bIsSelected = false;
public:
    USceneComponent();

    FMatrix GetModelMatrix() const;

    FMatrix GetQuatModelMatrix() const;

    //이동, 
    virtual void Update(float deltaTime);

    static UClass* StaticClass() {
        static UClass Class("USceneComponent", UObject::StaticClass());
        return &Class;
    }

    const FVector& GetLocation()const;
};

