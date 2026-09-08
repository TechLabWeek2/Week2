#pragma once
#include "UPrimitiveComponent.h"

class UFloorComp :
	public UPrimitiveComponent
{
public:
	int32 floorSizeX;
	int32 floorSizeY;
	UFloorComp** floorPlaneList;
public:
	UFloorComp();
	void ConstructFloor(FMeshResource* floor1, FMeshResource* floor2);
};