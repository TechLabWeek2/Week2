#pragma once
#include "USceneComponent.h"

class FMeshResource;
class URenderer;

class UPrimitiveComponent :
    public USceneComponent
{
    using Super = USceneComponent;

public:
    ETypePrimitive primitiveType;

    UPrimitiveComponent();

    void Update(float deltaTime) override;

    FMeshResource* GetMeshResource() const;
    void SetMeshResource(FMeshResource* pMeshResource);

private:
    //메시 데이터
    FMeshResource* MeshResource = nullptr;
};