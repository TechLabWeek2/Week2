#pragma once
#include "USceneComponent.h"
#include "URenderer.h"
class UPrimitiveComponent :
    public USceneComponent
{
public:
    ETypePrimitive primitiveType;

    UPrimitiveComponent();

    virtual void Render(URenderer* Renderer) = 0;
};