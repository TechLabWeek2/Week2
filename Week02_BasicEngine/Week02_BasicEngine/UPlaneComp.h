#pragma once
#include "UPrimitiveComponent.h"
class UPlaneComp :
    public UPrimitiveComponent
{
public:
    UPlaneComp();

	static UClass* StaticClass() {
		static UClass Class("UPlaneComp", UPrimitiveComponent::StaticClass(), &UPlaneComp::CreateObject);
		return &Class;
	}

	static UObject* CreateObject()
	{
		return new UPlaneComp();
	}
};