#include "UCameraComp.h"
#include "FMatrix.h"
UCameraComp::UCameraComp()
{
	FOV = 30.0f;
}

UCameraComp::~UCameraComp()
{
}

FMatrix UCameraComp::GetViewMatrix() const
{
	return FMatrix::GetViewMatrix(RelativeLocation, RelativeRotation);
}

FMatrix UCameraComp::GetProjectionMatrix() const
{
	return FMatrix::GetProjectionMatrix(DegreeToRadian(FOV), AspectRatio, NearClip, FarClip, IsOrthogonal);
}
