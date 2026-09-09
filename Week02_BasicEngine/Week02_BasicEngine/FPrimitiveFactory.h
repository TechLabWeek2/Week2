#pragma once

#include "UCubeComp.h"
#include "USphereComp.h"
#include "UPlaneComp.h"
#include "FMeshResourceRegistry.h"
#include <FObjectFactory.h>

class FShaderResource;

//프리미티브 생성 전용 팩토리
class FPrimitiveFactory
{
public:
	static UPrimitiveComponent* CreatePrimitive(ETypePrimitive type, FShaderResource* shaderResource, const FMeshResourceRegistry& MeshRegistry)
	{
		FMeshResource* MeshResource = MeshRegistry.GetMeshResource(type);

		if (MeshResource == nullptr || shaderResource == nullptr)
		{
			return nullptr;
		}

		UClass* PrimitiveClass = nullptr;

		switch (type)
		{
		case ETypePrimitive::Cube:
			PrimitiveClass = UCubeComp::StaticClass();
			break;
		case ETypePrimitive::Sphere:
			PrimitiveClass = USphereComp::StaticClass();
			break;
		case ETypePrimitive::Plane:
			PrimitiveClass = UPlaneComp::StaticClass();
			break;

		default:
			return nullptr;
		}

		UObject* NewObject =
			FObjectFactory::ConstructObject(PrimitiveClass);

		if (!NewObject)
		{
			return nullptr;
		}

		UPrimitiveComponent* NewPrimitive =
			static_cast<UPrimitiveComponent*>(NewObject);

		NewPrimitive->SetMeshResource(MeshResource);
		NewPrimitive->SetShaderResource(shaderResource);

		return NewPrimitive;
	}

};
