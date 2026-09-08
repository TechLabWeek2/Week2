#pragma once

#include <d3d11.h>
#include "Shapes.h"
#include "Core/Core.h"

//struct ResourceData;

class FMeshResource
{
public:
	//VertexBuffer, IndexBuffer 생성
	void CreateVertexBuffer();

	D3D11_PRIMITIVE_TOPOLOGY GetTopology()const;

	void Release();

	//버텍스 버퍼 세팅 반환
	virtual D3D11_BUFFER_DESC GetBufferDesc() const;

	//초기화
	virtual void Initialize();

	void SetMeshResourceData(ResourceData& pData);

	ID3D11Buffer* GetVertexBuffer()const;

	UINT GetNumVertices()const;

	const TArray<FVertexSimple>& GetVertices()const;

private:
	//Vertices 등이 담긴 메시 데이터
	ResourceData MeshResourceData;
	UINT numVertices = 0;
	ID3D11Buffer* VertexBuffer = nullptr;
	ID3D11Buffer* IndexBuffer = nullptr;
};

