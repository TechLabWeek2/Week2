#include "FTriangleResource.h"

void FTriangleResource::Initialize()
{
	Vertices = triangle_vertices;
	Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Super::Initialize();
}
