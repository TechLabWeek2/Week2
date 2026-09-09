#include "USceneComponent.h"
#include "Utils/Math.h"

#include <iostream>

USceneComponent::USceneComponent()
{
	Class = StaticClass();
	RelativeLocation = FVector(0.0f, 0.0f, 0.0f);
	RelativeRotation = FVector(0.0f, 0.0f, 0.0f);
	RelativeScale3D = FVector(0.1f, 0.1f, 0.1f);
	RelativeQ = RelativeQ.FromEuler(RelativeRotation);
}

FMatrix USceneComponent::GetModelMatrix() const
{
	//return FMatrix::GetModelMatrix(RelativeLocation, RelativeRotation, RelativeScale3D);
	FMatrix scaleMatrix =
		FMatrix::Scaling(RelativeScale3D);

	FMatrix rotationMatrix =
		FMatrix::Rotation(RelativeRotation);

	FMatrix translationMatrix =
		FMatrix::Translation(RelativeLocation);
	return scaleMatrix * rotationMatrix * translationMatrix;
}

FMatrix USceneComponent::GetQuatModelMatrix() const
{
	FMatrix scaleMatrix =
		FMatrix::Scaling(RelativeScale3D);

	FMatrix rotationMatrix = RelativeQ.ToMatrix();

	FMatrix translationMatrix =
		FMatrix::Translation(RelativeLocation);
	return scaleMatrix * rotationMatrix * translationMatrix;
};

void USceneComponent::Update(float deltaTime)
{
}

const FVector& USceneComponent::GetLocation() const
{
	return RelativeLocation;
}
