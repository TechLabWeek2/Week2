#include "FCubeResource.h"

void FCubeResource::Initialize()
{
	Vertices = cube_vertices;
	Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	Super::Initialize();
}
