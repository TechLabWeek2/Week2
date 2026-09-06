#include "UCubeComp.h"

UCubeComp::UCubeComp()
{

}

void UCubeComp::Render(URenderer* Renderer)
{
	Renderer->UpdateConstant(this->GetTransformMatrix() * Renderer->MainCamera->GetViewMatrix() * Renderer->MainCamera->GetProjectionMatrix());
	Renderer->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Renderer->RenderPrimitive(Vertices, NumVertices);
}
