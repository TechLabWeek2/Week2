#pragma once 


#define KINDA_SMALL_NUMBER	(1.e-4f)

constexpr float PI = 3.14159265358979323846f;

struct FVector;

template<typename T>
inline T Abs(const T A)
{
	return (A < (T)0) ? (T)-A : A;
}

float DegreeToRadian(float degree);
float RadianToDegree(float radian);