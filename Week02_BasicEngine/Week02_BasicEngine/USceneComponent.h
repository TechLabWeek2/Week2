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
    bool bIsSelected = false;
public:
    USceneComponent();
    virtual ~USceneComponent() {}

    FMatrix GetModelMatrix() const;

    //이동, 
    virtual void Update();

    static UClass* StaticClass() {
        static UClass Class("USceneComponent", UObject::StaticClass());
        return &Class;
    }

    const FVector& GetLocation()const;
};

