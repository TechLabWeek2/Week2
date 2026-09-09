#pragma once
#include "UPrimitiveComponent.h"
class USphereComp :
    public UPrimitiveComponent
{
public:
    USphereComp();

	static UClass* StaticClass() {
		static UClass Class("USphereComp", UPrimitiveComponent::StaticClass(), &USphereComp::CreateObject);
		return &Class;
	}
	static UObject* CreateObject()
	{
		return new USphereComp();
	}
};

