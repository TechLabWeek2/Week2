#pragma once
#include "UObject.h"
#include "UCameraComp.h"
#include "FVector.h";

class UPicking
{
public:
	float ndcX;
	float ndcY;
	float ndcZ;
	float ndcW;
	float viewX;
	float viewY;
	float viewZ;
	float worldX;
	float worldY;
	float worldZ;
	FVector forward;
	FVector right;
	FVector up;
	FVector dirToWorld;
	const float thresholdRatio = 100.f;

public:
	static UObject* GetPickedObject(float ndcX, float ndcY, UCameraComp* &Camera, FVector forward, FVector right, FVector up, UObject** &ObjectList, int32 ObjectCnt, bool* bIsPicking);
};