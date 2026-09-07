#include "UPicking.h"

USceneComponent* UPicking::GetPickedComponent(float ndcX, float ndcY, UCameraComp* &Camera, FVector forward, FVector right, FVector up, USceneComponent** &SceneComponentList, int32 SceneComponentCnt, bool* bIsPicking)
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

    float distanceMin = 10000.f;
    bool bIsFound = false;

    USceneComponent* pickedObject = nullptr;

    for (int32 i = 0; i < SceneComponentCnt; i++)
    {
        FVector componentLocation(SceneComponentList[i]->RelativeLocation.x, SceneComponentList[i]->RelativeLocation.y, SceneComponentList[i]->RelativeLocation.z);
        FVector difference = componentLocation - Camera->RelativeLocation; // camera -> component 벡터
        if (difference.Dot(dirToWorld) < 0) // 오브젝트가 카메라 뒤에 있으면 무시
        {
            continue;
        }
        float distanceRay = difference.Cross(dirToWorld).Size() / dirToWorld.Size(); // component에서 Ray까지의 최단거리
        float distanceCamera = (Camera->RelativeLocation - componentLocation).Size(); // component에서 camera까지의 거리
        distanceCamera = distanceCamera < 0.001f ? 0.001f : distanceCamera; 
        if (distanceRay < SceneComponentList[i]->RelativeScale3D.Size() && distanceMin > distanceCamera)
        {
            *bIsPicking = true;
            bIsFound = true;
            distanceMin = distanceCamera;
            pickedObject = SceneComponentList[i];
        }
    }
    if (bIsFound == false) // 선택된 오브젝트가 없음
    {
        *bIsPicking = false;
        pickedObject = nullptr;
    }
    return pickedObject;
}