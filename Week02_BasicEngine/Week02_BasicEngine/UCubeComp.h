#pragma once
#include "UPrimitiveComponent.h"

class UCubeComp :
    public UPrimitiveComponent
{
public:
    UCubeComp();

    virtual void Render(URenderer* Renderer) override;
};

