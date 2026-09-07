#include "FVector.h"

FVector4 FVector4::operator*(const FMatrix& M) const
{
    FVector4 result(0, 0, 0, 0);

    result.x = x * M.m[0][0] + y * M.m[1][0] + z * M.m[2][0] + w * M.m[3][0];
    result.y = x * M.m[0][1] + y * M.m[1][1] + z * M.m[2][1] + w * M.m[3][1];
    result.z = x * M.m[0][2] + y * M.m[1][2] + z * M.m[2][2] + w * M.m[3][2];
    result.w = x * M.m[0][3] + y * M.m[1][3] + z * M.m[2][3] + w * M.m[3][3];

    return result;
}

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