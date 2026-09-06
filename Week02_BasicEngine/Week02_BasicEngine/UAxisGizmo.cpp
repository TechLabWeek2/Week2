#include "UAxisGizmo.h"

UAxisGizmo::UAxisGizmo()
{
}

void UAxisGizmo::Render(URenderer* Renderer)
{
	RelativeLocation = FVector(0, 0, 0);
	RelativeRotation = FVector(0, 0, 0);
	RelativeScale3D = FVector(10.f, 10.f, 10.f);
	Renderer->UpdateConstant(this->GetTransformMatrix() * Renderer->MainCamera->GetViewMatrix() * Renderer->MainCamera->GetProjectionMatrix());
	Renderer->DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Renderer->RenderPrimitive(Vertices, NumVertices);

	
	RelativeLocation = FVector(-0.9f, -0.9f, 0);
	RelativeRotation = Renderer->MainCamera->RelativeRotation;
	RelativeRotation.x = RelativeRotation.x;
	RelativeRotation.y = -RelativeRotation.y - (3.14/2);
	RelativeRotation.z = RelativeRotation.z;
	RelativeScale3D = FVector(0.05f, 0.05f, 0.05f);
	Renderer->UpdateConstant(this->GetTransformMatrix());
	Renderer->RenderPrimitive(Vertices, NumVertices);
}
