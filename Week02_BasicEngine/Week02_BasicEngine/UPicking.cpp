#include "UPicking.h"

UObject* GetPickedObject(float ndcX, float ndcY, UCameraComp* Camera, FVector forward, FVector right, FVector up, UObject** ObjectList, int32 ObjectCnt, bool* bIsPicking)
{
    // picking
    ndcX = ndcX;  // screen xy to NDC xy
    ndcY = ndcY;
    float ndcZ = 0.f;
    float ndcW = 1.f;

    float viewX = Camera->AspectRatio * tanf(DegreeToRadian(Camera->FOV / 2)) * ndcX;
    float viewY = tanf(DegreeToRadian(Camera->FOV / 2)) * ndcY;
    float viewZ = ndcW;

    float worldX = right.x * viewX + up.x * viewY + forward.x * viewZ;
    float worldY = right.y * viewX + up.y * viewY + forward.y * viewZ;
    float worldZ = right.z * viewX + up.z * viewY + forward.z * viewZ;

    FVector dirToWorld(worldX, worldY, worldZ);
    dirToWorld.Normalize();

    const float thresholdRatio = 100.f;
    float distanceMin = 10000.f;
    bool bIsFound = false;

    UObject* pickedObject = nullptr;

    for (int32 i = 0; i < ObjectCnt; i++)
    {
        //FVector primitiveLocation(ObjectList[i]->Location.x, ObjectList[i]->Location.y, ObjectList[i]->Location.z);
        FVector primitiveLocation;
        FVector difference = primitiveLocation - Camera->RelativeLocation;
        if (difference.Dot(dirToWorld) < 0) // 오브젝트가 카메라 뒤에 있음
        {
            continue;
        }
        float distanceRay = difference.Cross(dirToWorld).Size() / dirToWorld.Size();
        float distanceCamera = (Camera->RelativeLocation - primitiveLocation).Size();
        distanceCamera = distanceCamera < 0.001f ? 0.001f : distanceCamera;
        if (distanceRay < thresholdRatio / distanceCamera && distanceMin > distanceCamera)
        {
            *bIsPicking = true;
            bIsFound = true;
            distanceMin = distanceCamera;
            //pickedObject = ObjectList[i];  // 수정필요
        }
    }
    if (bIsFound == false) // 선택된 오브젝트가 없음
    {
        *bIsPicking = false;
        pickedObject = nullptr;
    }
    return pickedObject;
}