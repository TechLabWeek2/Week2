#pragma once
#include "UPrimitiveComponent.h"

class ID3D11Buffer;
class ID3D11Buffer;

class UCubeComp :
    public UPrimitiveComponent
{
public:
    UCubeComp();

	static UClass* StaticClass() {
		static UClass Class("USphere", UPrimitiveComponent::StaticClass());
		return &Class;
	}
};

