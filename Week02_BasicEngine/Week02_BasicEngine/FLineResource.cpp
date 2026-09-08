#include "FLineResource.h"

void FLineResource::Initialize()
{
	Vertices = line_vertices;
	Topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	Super::Initialize();
}
