#pragma once

#include "UCubeComp.h"
#include "USphereComp.h"
#include "UPlaneComp.h"
#include "FMeshResourceRegistry.h"

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

		UPrimitiveComponent* NewPrimitive = nullptr;

		switch (type)
		{
		case ETypePrimitive::Cube:
			NewPrimitive = new UCubeComp();
			break;
		case ETypePrimitive::Sphere:
			NewPrimitive = new USphereComp();
			break;
		case ETypePrimitive::Plane:
			NewPrimitive = new UPlaneComp();
			break;

		default:
			return nullptr;
		}

		NewPrimitive->SetMeshResource(MeshResource);
		NewPrimitive->SetShaderResource(shaderResource);

		return NewPrimitive;
	}

};
