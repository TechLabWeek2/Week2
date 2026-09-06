#include "USceneComponent.h"

USceneComponent::USceneComponent()
{
	RelativeLocation = FVector(0.0f, 0.0f, 0.0f);
	RelativeRotation = FVector(0.0f, 0.0f, 0.0f);
	RelativeScale3D = FVector(0.1f, 0.1f, 0.1f);
}

FMatrix USceneComponent::GetTransformMatrix() const
{
	FMatrix scaleMatrix =
		FMatrix::Scaling(RelativeScale3D);

	FMatrix rotationMatrix =
		FMatrix::Rotation(RelativeRotation);

	FMatrix translationMatrix =
		FMatrix::Translation(RelativeLocation);
	return scaleMatrix * rotationMatrix * translationMatrix;
}
