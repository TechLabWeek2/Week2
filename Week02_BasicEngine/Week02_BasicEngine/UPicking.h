#pragma once
#include "UPrimitiveComponent.h"
#include "UCameraComp.h"
#include "FMatrix.h"
#include "FMeshResource.h"
#include "Shapes.h"
#include "TArray.h"

class UPicking
{
public:
	//float ndcX;
	//float ndcY;
	//float ndcZ;
	//float ndcW;
	//float viewX;
	//float viewY;
	//float viewZ;
	//float worldX;
	//float worldY;
	//float worldZ;
	//FVector forward;
	//FVector right;
	//FVector up;
	//FVector dirToWorld;
	FVector DragStartLocation;
	FVector DragStartIntersection;
	FVector DragAxis;
	FVector DragPlaneNormal;
public:
	static UPrimitiveComponent* GetPickedPrimitive(float ndcX, float ndcY, UCameraComp* &Camera, FVector forward, FVector right, FVector up, const TArray<UObject*> &PrimitiveComponentList, int32 PrimitiveComponentCnt, bool* bIsPicking);
};