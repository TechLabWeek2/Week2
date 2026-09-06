#include "UCameraComp.h"

UCameraComp::UCameraComp()
{
	FOV = 60.0f;
}

UCameraComp::~UCameraComp()
{
}

FMatrix UCameraComp::GetViewMatrix() const
{
	FVector ZAxis(cos(RelativeRotation.y)*cos(RelativeRotation.x), sin(RelativeRotation.x), -sin(RelativeRotation.y)*cos(RelativeRotation.x));
	ZAxis.Normalize();
	FVector XAxis = FVector(0,1,0).Cross(ZAxis);
	XAxis.Normalize();
	FVector YAxis = ZAxis.Cross(XAxis);
	YAxis.Normalize();
	FMatrix M;
	M.m[0][0] = XAxis.x;
	M.m[0][1] = YAxis.x;
	M.m[0][2] = ZAxis.x;
	M.m[0][3] = 0.0f;
	M.m[1][0] = XAxis.y;
	M.m[1][1] = YAxis.y;
	M.m[1][2] = ZAxis.y;
	M.m[1][3] = 0.0f;
	M.m[2][0] = XAxis.z;
	M.m[2][1] = YAxis.z;
	M.m[2][2] = ZAxis.z;
	M.m[2][3] = 0.0f;
	M.m[3][0] = -RelativeLocation.x * XAxis.x - RelativeLocation.y * XAxis.y - RelativeLocation.z * XAxis.z;
	M.m[3][1] = -RelativeLocation.x * YAxis.x - RelativeLocation.y * YAxis.y - RelativeLocation.z * YAxis.z;
	M.m[3][2] = -RelativeLocation.x * ZAxis.x - RelativeLocation.y * ZAxis.y - RelativeLocation.z * ZAxis.z;
	M.m[3][3] = 1.0f;
	return M;
}

FMatrix UCameraComp::GetProjectionMatrix() const
{
	FMatrix M;
	M.m[0][0] = 1 / (tan(FOV)) * AspectRatio;
	M.m[1][1] = 1 / tan(FOV);
	M.m[2][2] = (FarClip) / (FarClip - NearClip);
	M.m[2][3] = 1;
	M.m[3][2] = -(FarClip * NearClip) / (FarClip - NearClip);
	M.m[3][3] = 0;
	return M;
}
