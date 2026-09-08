#include "UFloorComp.h"

UFloorComp::UFloorComp()
{
	primitiveType = ETypePrimitive::Floor;
	floorSizeX = 50;
	floorSizeY = 50;
    floorPlaneList = nullptr;
}

void UFloorComp::ConstructFloor(FMeshResource* floor1, FMeshResource* floor2)
{
    UFloorComp** floorPlaneList = new UFloorComp * [floorSizeX * floorSizeY];
    for (int32 i = 0; i < floorSizeX; i++)
    {
        for (int32 j = 0; j < floorSizeY; j++)
        {
            floorPlaneList[i + j] = new UFloorComp();
            floorPlaneList[i + j]->primitiveType = ETypePrimitive::Floor;
            if ((i + j) % 2 == 0)
            {
                floorPlaneList[i + j]->SetMeshResource(floor1);
            }
            else
            {
                floorPlaneList[i + j]->SetMeshResource(floor2);
            }
            floorPlaneList[i + j]->RelativeScale3D = FVector(.1f, .1f, .1f);
            floorPlaneList[i + j]->RelativeLocation = FVector((float)i * .2f - 5.f, 0.f, (float)j * .2f - 5.f);
            floorPlaneList[i + j]->RelativeRotation = FVector(DegreeToRadian(90), 0.f, 0.f);

            floorPlaneList[i + j]->SetBlendMode(BlendMode::Alpha);
        }
    }
}