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

    FVector rayVector(worldX, worldY, worldZ);
    rayVector.Normalize();

    float distanceMin = 10000.f;
    bool bIsFound = false;

    USceneComponent* pickedObject = nullptr;

    for (int32 i = 0; i < SceneComponentCnt; i++)
    {
        FVector componentLocation(SceneComponentList[i]->RelativeLocation.x, SceneComponentList[i]->RelativeLocation.y, SceneComponentList[i]->RelativeLocation.z);
        FVector difference = componentLocation - Camera->RelativeLocation; // camera -> component 벡터
        if (difference.Dot(rayVector) < 0) // 오브젝트가 카메라 뒤에 있으면 무시
        {
            continue;
        }
        float distanceRay = difference.Cross(rayVector).Size() / rayVector.Size(); // component에서 Ray까지의 최단거리
        if (distanceRay < SceneComponentList[i]->RelativeScale3D.Size()) // Sphere Boundary 체크로 1차 거르기
        {
            FVertexSimple* targetVertices;
            int32 numVertices;
            switch (SceneComponentList[i]->primitiveType) // 각각의 맞는 xxxxx_vertices[]를 로드
            {
            case EPT_Sphere:
                targetVertices = sphere_vertices;
                numVertices = sizeof(sphere_vertices) / sizeof(FVertexSimple);
                break;
            case EPT_Cube:
                targetVertices = cube_vertices;
                numVertices = sizeof(cube_vertices) / sizeof(FVertexSimple);
                break;
            }
            //////////////////////////////////////
            targetVertices = cube_vertices; // 임시
            numVertices = sizeof(cube_vertices) / sizeof(FVertexSimple); // 임시
            //////////////////////////////////////

            // 로컬 좌표를 월드좌표로 변환해야함
            FMatrix transformMatrix = SceneComponentList[i]->GetModelMatrix();
            for (uint32 j = 0; j < numVertices; j++)
            {
                FVector targetVerticesLocal(targetVertices[j].x, targetVertices[j].y, targetVertices[j].z);
                //FVector targetVerticesWorld = targetVerticesLocal * transformMatrix;
            }

            for (uint32 j = 0; j < numVertices; j+=3) // Moller-Trumbore 알고리즘
            {
                float epslion = 0.001f;

                FVector v0(targetVertices[j].x, targetVertices[j].y, targetVertices[j].z);
                FVector v1(targetVertices[j + 1].x, targetVertices[j + 1].y, targetVertices[j + 1].z);
                FVector v2(targetVertices[j + 2].x, targetVertices[j + 2].y, targetVertices[j + 2].z);

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
                FVector s = Camera->RelativeLocation - v0;
                float u = inverseDet * s.Dot(rayCrossEdge2);
                if (u < -epslion || u > 1 + epslion) // u 는 0 ~ 1 이어야함
                {
                    continue;
                }

                FVector sCroosEdge1 = s.Cross(edge1);
                float v = inverseDet * rayVector.Dot(sCroosEdge1);
                if (v < -epslion || u + v > 1 + epslion) // v 는 0 ~ 1, u + v < 1 이어야함
                {
                    continue;
                }

                float t = inverseDet * edge2.Dot(sCroosEdge1);
                if (t < epslion) // t가 음수이면 광선방향의 뒷쪽임
                {
                    continue;
                }
                FVector rayCastedLocation = Camera->RelativeLocation + rayVector * t;
                float distanceCamera = (Camera->RelativeLocation - rayCastedLocation).Size(); // 충돌지점에서 camera까지의 거리
                distanceCamera = distanceCamera < 0.001f ? 0.001f : distanceCamera;
                if (distanceMin > distanceCamera)
                {
                    *bIsPicking = true;
                    bIsFound = true;
                    distanceMin = distanceCamera;
                    pickedObject = SceneComponentList[i];
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