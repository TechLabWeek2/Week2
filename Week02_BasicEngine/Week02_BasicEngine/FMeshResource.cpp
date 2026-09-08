#include "FMeshResource.h"
#include "FGraphicsDevice.h"

void FMeshResource::CreateVertexBuffer()
{
	//numVertices = sizeof(Vertices);
	numVertices = MeshResourceData.Vertices.Num();

	//D3D11_BUFFER_DESC vertexbufferdesc = {};
	//vertexbufferdesc.ByteWidth = numVertices;
	//vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated;
	//vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_BUFFER_DESC vertexbufferdesc = GetBufferDesc();
	D3D11_SUBRESOURCE_DATA vertexbufferSRD = { MeshResourceData.Vertices.Data()};

	HRESULT hr = GGraphicsDevice.GetDevice()->CreateBuffer(&vertexbufferdesc, &vertexbufferSRD, &VertexBuffer);
	if (FAILED(hr))
	{
		assert(false);
	}
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

D3D11_BUFFER_DESC FMeshResource::GetBufferDesc() const
{
	D3D11_BUFFER_DESC vertexbufferdesc = {};
	vertexbufferdesc.ByteWidth = numVertices * sizeof(FVertexSimple);
	vertexbufferdesc.Usage = D3D11_USAGE_IMMUTABLE; // will never be updated;
	vertexbufferdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	return vertexbufferdesc;
}

void FMeshResource::Initialize()
{
	CreateVertexBuffer();
}

void FMeshResource::SetMeshResourceData(ResourceData& pData)
{
	MeshResourceData = pData;
}

ID3D11Buffer* FMeshResource::GetVertexBuffer() const
{
	return VertexBuffer;
}

UINT FMeshResource::GetNumVertices() const
{
	return numVertices;
}

const TArray<FVertexSimple>& FMeshResource::GetVertices() const
{
	return MeshResourceData.Vertices;
}

D3D11_PRIMITIVE_TOPOLOGY FMeshResource::GetTopology() const
{
	return MeshResourceData.Topology;
}

