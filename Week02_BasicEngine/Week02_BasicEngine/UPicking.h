#pragma once
#include "USceneComponent.h"
#include "UCameraComp.h"
#include "FVector.h"
#include "FMatrix.h"
#include "FVertexSimple.h"
#include "Shapes.h"

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

public:
	static USceneComponent* GetPickedComponent(float ndcX, float ndcY, UCameraComp* &Camera, FVector forward, FVector right, FVector up, USceneComponent** &SceneComponentList, int32 SceneComponentCnt, bool* bIsPicking);
};