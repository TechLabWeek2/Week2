#include "FMeshResource.h"
#include "FGraphicsDevice.h"

//void FMeshResource::CreateVertexBuffer(TArray<FVertexSimple>& vertices, UINT VerticesNum)
void FMeshResource::CreateVertexBuffer(FVertexSimple* vertices, UINT VerticesNum)
{
	//UINT VerticesCount = sizeof(*vertices);
	/*UINT SimpleCount = sizeof(FVertexSimple);
	numVertices = VerticesCount / SimpleCount;*/
	numVertices = VerticesNum;
	//numVertices = vertices.Num();

	D3D11_BUFFER_DESC vertexbufferdesc = {};
	//vertexbufferdesc.ByteWidth = sizeof(FVertexSimple);
	vertexbufferdesc.ByteWidth = VerticesNum;
	vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexbufferSRD = { vertices};

	GGraphicsDevice.GetDevice()->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &VertexBuffer);
}

void FMeshResource::SetTopology(D3D11_PRIMITIVE_TOPOLOGY pTopology)
{
	Topology = pTopology;
}

void FMeshResource::Release()
{
	if (VertexBuffer)
	{
		VertexBuffer->Release();
		VertexBuffer = nullptr;
	}
	if (IndexBuffer)
	{
		IndexBuffer->Release();
		IndexBuffer = nullptr;
	}
}

