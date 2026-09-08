#pragma once

#include <d3d11.h>
#include "Shapes.h"
#include "Core/Core.h"

struct FVertexSimple;

class FMeshResource
{
public:	
	//VertexBuffer, IndexBuffer 생성
	//void CreateVertexBuffer(TArray<FVertexSimple>& vertices, UINT VerticesNum);
	void CreateVertexBuffer(FVertexSimple* vertices, UINT VerticesNum);
	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY pTopology);

	void Release();

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	UINT numVertices;
	D3D11_PRIMITIVE_TOPOLOGY Topology;
};