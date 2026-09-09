#include "UPicking.h"

UPrimitiveComponent* UPicking::GetPickedPrimitive(float ndcX, float ndcY, UCameraComp* &Camera, FVector forward, FVector right, FVector up, const TArray<UObject*>& ObjectList, int32 ObjectListCnt, bool* bIsPicking)
{
    // picking
    float ndcZ = 0.f;
    float ndcW = 1.f;

    float viewX = 0.f;
    float viewY = 0.f;
    float viewZ = ndcW;

    float worldX = 0.f;
    float worldY = 0.f;
    float worldZ = 0.f;

    FVector rayOrigin;
    FVector rayVector;

    if (Camera->IsOrthogonal)
    {
        FMatrix Proj = Camera->GetProjectionMatrix();
        viewX = ndcX / Proj.m[0][0];
        viewY = ndcY / Proj.m[1][1];

        rayOrigin = Camera->RelativeLocation + right * viewX + up * viewY;
        rayVector = forward;
        rayVector.Normalize();
    }
    else
    {
        FMatrix Proj = Camera->GetProjectionMatrix();
        viewX = ndcX / Proj.m[0][0];
        viewY = ndcY / Proj.m[1][1];

        worldX = right.x * viewX + up.x * viewY + forward.x * viewZ;
        worldY = right.y * viewX + up.y * viewY + forward.y * viewZ;
        worldZ = right.z * viewX + up.z * viewY + forward.z * viewZ;

        rayOrigin = Camera->RelativeLocation;
        rayVector = FVector(worldX, worldY, worldZ);
        rayVector.Normalize();
    }

    float distanceMin = 10000.f;
    bool bIsFound = false;

    UPrimitiveComponent* pickedObject = nullptr;

    UPrimitiveComponent* PrimitiveComponent = nullptr;
    for (int32 i = 1; i < ObjectListCnt; i++)
    {
        PrimitiveComponent = static_cast<UPrimitiveComponent*>(ObjectList[i]);
        FVector componentLocation(PrimitiveComponent->RelativeLocation.x, PrimitiveComponent->RelativeLocation.y, PrimitiveComponent->RelativeLocation.z);
        FVector difference = componentLocation - rayOrigin; // camera -> component 벡터

        if (!(PrimitiveComponent->primitiveType == ETypePrimitive::Gizmo || PrimitiveComponent->primitiveType == ETypePrimitive::Plane || PrimitiveComponent->primitiveType == ETypePrimitive::Cube || PrimitiveComponent->primitiveType == ETypePrimitive::Sphere))
        {
            continue;
        }
        if (difference.Dot(rayVector) < 0 || !PrimitiveComponent->bIsActive) // 오브젝트가 카메라 뒤에 있으면 무시
        {
            continue;
        }
        float distanceRay = difference.Cross(rayVector).Size() / rayVector.Size(); // component에서 Ray까지의 최단거리
        if (distanceRay < PrimitiveComponent->RelativeScale3D.Size() * sqrt(3)) // Sphere Boundary 체크로 1차 거르기
        {
            int32 numVertices = 0;
            const TArray<FVertexSimple>& targetVertices = PrimitiveComponent->GetMeshResource()->GetVertices();
            numVertices = PrimitiveComponent->GetMeshResource()->GetNumVertices();

            FMatrix transformMatrix = PrimitiveComponent->GetQuatModelMatrix();
            for (int32 j = 0; j < numVertices; j+=3) // Moller-Trumbore 알고리즘
            {
                float epslion = KINDA_SMALL_NUMBER;

                FVertexSimple currentVertices[3];
                for (int32 k = 0; k < 3; k++) // 로컬 좌표를 월드좌표로 변환해야함
                {
                    FVector4 targetVerticesLocal(targetVertices[j + k].x, targetVertices[j + k].y, targetVertices[j + k].z, 1);
                    FVector4 targetVerticesWorld = targetVerticesLocal * transformMatrix;
                    currentVertices[k].x = targetVerticesWorld.x;
                    currentVertices[k].y = targetVerticesWorld.y;
                    currentVertices[k].z = targetVerticesWorld.z;
                }

                FVector v0(currentVertices[0].x, currentVertices[0].y, currentVertices[0].z);
                FVector v1(currentVertices[1].x, currentVertices[1].y, currentVertices[1].z);
                FVector v2(currentVertices[2].x, currentVertices[2].y, currentVertices[2].z);

                FVector edge1 = v1 - v0;
                FVector edge2 = v2 - v0;

                FVector normal = edge1.Cross(edge2); // 해당 삼각형의 normal vector
                if (normal.Dot(rayVector) > 0) // backspace culling
                {
                    continue;
                }
                
                FVector rayCrossEdge2 = rayVector.Cross(edge2); 
                float det = rayCrossEdge2.Dot(edge1);
                if (abs(det) < epslion) // 해당 삼각형이 ray와 거의 평행한 경우
                {
                    continue;
                }

                float inverseDet = 1 / det;
                FVector s = rayOrigin - v0;
                float u = inverseDet * s.Dot(rayCrossEdge2);
                if (u < -epslion || u > 1 + epslion) // u 는 0 ~ 1 이어야함
                {
                    continue;
                }

                FVector sCrossEdge1 = s.Cross(edge1);
                float v = inverseDet * rayVector.Dot(sCrossEdge1);
                if (v < -epslion || u + v > 1 + epslion) // v 는 0 ~ 1, u + v < 1 이어야함
                {
                    continue;
                }

                float t = inverseDet * edge2.Dot(sCrossEdge1);
                if (t < epslion) // t가 음수이면 광선방향의 뒷쪽임
                {
                    continue;
                }
                FVector rayCastedLocation = rayOrigin + rayVector * t;
                float distanceCamera = (rayOrigin - rayCastedLocation).Size(); // 충돌지점에서 camera까지의 거리
                distanceCamera = distanceCamera < 0.001f ? 0.001f : distanceCamera;
                if (distanceMin > distanceCamera)
                {
                    *bIsPicking = true;
                    bIsFound = true;
                    distanceMin = distanceCamera;
                    pickedObject = PrimitiveComponent;
                    if (PrimitiveComponent->primitiveType == ETypePrimitive::Gizmo)
                    {
                        return pickedObject;
                    }
                }
            }
        }
    }
    if (bIsFound == false) // 선택된 오브젝트가 없음
    {
        *bIsPicking = false;
        pickedObject = nullptr;
    }
    return pickedObject;
}

void UPicking::Hovering(float ndcX, float ndcY, UCameraComp*& Camera, FVector forward, FVector right, FVector up, const TArray<UObject*>& PrimitiveComponentList, int32 PrimitiveComponentCnt, bool* bIsPicking, UPrimitiveComponent*& hoveringtObjectPtr, UPrimitiveComponent*& prevObjectPtr, UPrimitiveComponent*& pickedObjectPtr, UPrimitiveComponent*& pickedGizmoPtr)
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
            if (prevObjectPtr != pickedObjectPtr)
            {
                prevObjectPtr->bIsSelected = false;
            }
            if (pickedGizmoPtr)
            {
                if (prevObjectPtr != pickedGizmoPtr)
                {
                    prevObjectPtr->bIsSelected = false;
                }
            }
        }
    }
}