#include "UCameraComp.h"
#include "FMatrix.h"
UCameraComp::UCameraComp()
{
	FOV = 90.0f;
}

UCameraComp::~UCameraComp()
{
}

FMatrix UCameraComp::GetViewMatrix() const
{
	//return FMatrix::GetViewMatrix(RelativeLocation, RelativeRotation);
	return FMatrix::GetViewMatrix_UEToDX(RelativeLocation, RelativeRotation);
}

FMatrix UCameraComp::GetProjectionMatrix() const
{
	return FMatrix::GetProjectionMatrix(DegreeToRadian(FOV / 2.f), ZoomLevel, AspectRatio, NearClip, FarClip, IsOrthogonal);
}

//const FVector UCameraComp::GetForwardVector_UE() const
//{
//	FVector Forward(cos(RelativeRotation.y) * cos(RelativeRotation.z), 
//					-cos(RelativeRotation.y) * sin(RelativeRotation.z), 
//					sin(RelativeRotation.y));
//	Forward.Normalize();
//	return Forward;
//}
//
//const FVector UCameraComp::GetRightVector_UE() const
//{
//	FVector Right = FVector(0.f, 0.f, 1.f).Cross(GetForwardVector_UE());
//	Right.Normalize();
//	return Right;
//}
//
//const FVector UCameraComp::GetUpVector_UE() const
//{
//	FVector Forward = GetForwardVector_UE();
//	FVector Right = FVector(0.f, 0.f, 1.f).Cross(Forward);
//	Right.Normalize();
//	FVector Up = Forward.Cross(Right);
//	Up.Normalize();
//	return Up;
//}
