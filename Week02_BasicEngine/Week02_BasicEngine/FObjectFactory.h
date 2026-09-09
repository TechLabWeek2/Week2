#pragma once


#include "UClass.h"

class FObjectFactory
{
public:
	static UObject* ConstructObject(const UClass* InClass)
	{
		if (InClass)
		{
			return InClass->CreateInstance();
		}
		return nullptr;
	}
};