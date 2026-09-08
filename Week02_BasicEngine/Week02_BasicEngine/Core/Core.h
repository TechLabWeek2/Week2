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
	FrontCulling
};

enum BlendMode
{
	Opaque,
	Alpha
};