#include "Math.h"

float DegreeToRadian(float degree)
{
	return degree * PI / 180.0f; 
}

float RadianToDegree(float radian)
{
	return radian * 180.f / PI;
}