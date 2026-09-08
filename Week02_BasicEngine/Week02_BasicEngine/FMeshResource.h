#pragma once

#include <d3d11.h>
#include "Shapes.h"
#include "Core/Core.h"

class FMeshResource
{
public:
	//VertexBuffer, IndexBuffer 생성
	//void CreateVertexBuffer(TArray<FVertexSimple>& vertices, UINT VerticesNum);
	void CreateVertexBuffer(FVertexSimple* vertices, UINT VerticesNum);
	void CreateVertexBuffer();
	void SetTopology(D3D11_PRIMITIVE_TOPOLOGY pTopology);

	void Release();

	//버텍스 버퍼 세팅 반환
	virtual D3D11_BUFFER_DESC GetBufferDesc() const;

	//초기화
	virtual void Initialize();

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	UINT numVertices;
	D3D11_PRIMITIVE_TOPOLOGY Topology;

	TArray<FVertexSimple> Vertices;
};

