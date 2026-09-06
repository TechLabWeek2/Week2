#pragma once
#include "UPrimitiveComponent.h"
class USphereComp :
    public UPrimitiveComponent
{
public:
    USphereComp();

    virtual void Render(URenderer* Renderer) override;
};

