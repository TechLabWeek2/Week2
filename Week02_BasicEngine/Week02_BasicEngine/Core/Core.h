#pragma once

#include "FVector.h"
#include "FString.h"
#include "TArray.h"
#include "Utils/Math.h"
#include "CoreType.h"

enum RasterizerState
{
	Solid,
	WireFrame,
	FrontCulling,
	Solid_Culling_None,
	WireFrame_FrontCulling
};

enum BlendMode
{
	Opaque,
	Alpha
}; 

enum DepthStateMode
{
	Depth,
	NoDepth
};