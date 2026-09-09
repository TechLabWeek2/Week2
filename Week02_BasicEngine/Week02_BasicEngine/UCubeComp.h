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
		static UClass Class("UCubeComp", UPrimitiveComponent::StaticClass(), &UCubeComp::CreateObject);
		return &Class;
	}

	static UObject* CreateObject()
	{
		return new UCubeComp();
	}
};

