#pragma once

#include <d3d11.h>

struct FVertexSimple
{
	float x, y, z;    // Position
	float r, g, b, a; // Color
};

class FMeshResource
{
public:
	//VertexBuffer, IndexBuffer 생성
	void CreateVertexBuffer(FVertexSimple* vertices, UINT byteWidth);

	void Bind();

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;

};

