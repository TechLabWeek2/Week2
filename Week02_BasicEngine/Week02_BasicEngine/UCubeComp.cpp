#include "UCubeComp.h"

UCubeComp::UCubeComp()
{
	primitiveType = ETypePrimitive::Cube;
}

void UCubeComp::Render(URenderer* Renderer)
{
	Renderer->UpdateConstant(this->GetModelMatrix() * Renderer->MainCamera->GetViewMatrix() * Renderer->MainCamera->GetProjectionMatrix(), bIsSelected);
	Renderer->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Renderer->RenderPrimitive(Vertices, NumVertices);
}
