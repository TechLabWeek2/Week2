#pragma once

#include <Tmap.h>
#include <UPrimitiveComponent.h>

class FMeshResource;

class FMeshResourceRegistry
{
public:
	void Registry(ETypePrimitive type, FMeshResource* resource)
	{
		MeshResources.Add(type, resource);
	}

	FMeshResource* GetMeshResource(ETypePrimitive type) const
	{
		auto* resourcePtr = MeshResources.Find(type);
		if (resourcePtr)
		{
			return *resourcePtr;
		}
		return nullptr;
	}

private:
	TMap<ETypePrimitive, FMeshResource*> MeshResources;
};
