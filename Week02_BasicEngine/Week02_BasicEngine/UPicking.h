#pragma once
#include "UPrimitiveComponent.h"
#include "UCameraComp.h"
#include "FMatrix.h"
#include "FMeshResource.h"
#include "Shapes.h"
#include "TArray.h"
#include "UObjectArray.h"

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

	static void Hovering(float ndcX, float ndcY, UCameraComp*& Camera, FVector forward, FVector right, FVector up, const TArray<UObject*>& PrimitiveComponentList, int32 PrimitiveComponentCnt, bool* bIsPicking, UPrimitiveComponent*& hoveringtObjectPtr, UPrimitiveComponent*& prevObjectPtr)
	{
		if (hoveringtObjectPtr != prevObjectPtr && hoveringtObjectPtr != nullptr)
		{
			prevObjectPtr = hoveringtObjectPtr;
		}
		hoveringtObjectPtr = UPicking::GetPickedPrimitive(ndcX, ndcY, Camera, forward, right, up, GUObjectArray.GetAllObjects(), GUObjectArray.GetNum(), bIsPicking);
		if (hoveringtObjectPtr)// 오브젝트 위에 마우스가 있음 (호버)
		{
			hoveringtObjectPtr->bIsSelected = true;
		}
		else // 빈공간에 마우스가 있음
		{
			if (prevObjectPtr)
			{
				prevObjectPtr->bIsSelected = false;
			}
		}
	}
};