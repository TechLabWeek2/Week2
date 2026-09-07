#include "FVector.h"

bool FVector::Equals(const FVector& V, float Tolerance) const
{
    return Abs(x - V.x) <= Tolerance && Abs(y - V.y) <= Tolerance && Abs(z - V.z) <= Tolerance;
}

float FVector::Size() const
{
    return sqrt(x * x + y * y + z * z);
}

float FVector::SizeSquared() const
{
    return x * x + y * y + z * z;
}

float FVector::Length() const
{
    return Size();
}

float FVector::SquaredLength() const
{
    return SizeSquared();
}

bool FVector::IsNearlyZero(float Tolerance) const
{
    return
        Abs(x) <= Tolerance
        && Abs(y) <= Tolerance
        && Abs(z) <= Tolerance;
}

bool FVector::IsZero() const
{
    return x == 0.f && y == 0.f && z == 0.f;
}

FString FVector::ToString() const
{
    return FString::Printf(L"x = &f, y = %f, z = %f", x, y, z);
}

bool FVector2::Equals(const FVector2& V, float Tolerance) const
{
    return Abs(x - V.x) <= Tolerance && Abs(y - V.y) <= Tolerance;
}

float FVector2::Size() const
{
    return sqrt(x * x + y * y);
}

float FVector2::SizeSquared() const
{
    return x * x + y * y;
}

float FVector2::Length() const
{
    return Size();
}

float FVector2::SquaredLength() const
{
    return SizeSquared();
}

bool FVector2::IsNearlyZero(float Tolerance) const
{
    return
        Abs(x) <= Tolerance
        && Abs(y) <= Tolerance;
}

bool FVector2::IsZero() const
{
    return x == 0.f && y == 0.f;
}

FString FVector2::ToString() const
{
    return FString::Printf(L"x = &f, y = %f", x, y);
}



bool FVector4::Equals(const FVector4& V, float Tolerance) const
{
    return Abs(x - V.x) <= Tolerance && Abs(y - V.y) <= Tolerance && Abs(z - V.z) <= Tolerance && Abs(w - V.w) <= Tolerance;
}

float FVector4::Size() const
{
    return sqrt(x * x + y * y + z * z + w * w);
}

float FVector4::SizeSquared() const
{
    return x * x + y * y + z * z + w * w;
}

float FVector4::Length() const
{
    return Size();
}

float FVector4::SquaredLength() const
{
    return SizeSquared();
}

bool FVector4::IsNearlyZero(float Tolerance) const
{
    return
        Abs(x) <= Tolerance
        && Abs(y) <= Tolerance
        && Abs(z) <= Tolerance
        && Abs(w) <= Tolerance;
}

bool FVector4::IsZero() const
{
    return x == 0.f && y == 0.f && z == 0.f && w == 0.f;
}

FString FVector4::ToString() const
{
    return FString::Printf(L"x = &f, y = %f, z = &f, w = %f", x, y, z, w);
}