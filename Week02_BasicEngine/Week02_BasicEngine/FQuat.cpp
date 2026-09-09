#include "FQuat.h"
#include "FMatrix.h"
#include "FVector.h"

FQuat FQuat::FromEuler(const FVector& Euler)
{
    float Roll = Euler.x * 0.5f;
    float Pitch = Euler.y * 0.5f;
    float Yaw = Euler.z * 0.5f;

    FQuat Rx(
        sin(Roll),
        0.0f,
        0.0f,
        cos(Roll)
    );

    FQuat Ry(
        0.0f,
        sin(Pitch),
        0.0f,
        cos(Pitch)
    );

    FQuat Rz(
        0.0f,
        0.0f,
        sin(Yaw),
        cos(Yaw)
    );

    return Rz * Ry * Rx;
}

FVector FQuat::ToEuler(const FQuat& Q) const
{
    FVector Euler;

    // X (Roll)
    float sinr_cosp = 2.0f * (w * x + y * z);
    float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
    Euler.x = atan2(sinr_cosp, cosr_cosp);

    // Y (Pitch)
    float sinp = 2.0f * (w * y - z * x);

    if (fabs(sinp) >= 1.0f)
        Euler.y = copysign(PI / 2.0f, sinp);
    else
        Euler.y = asin(sinp);

    // Z (Yaw)
    float siny_cosp = 2.0f * (w * z + x * y);
    float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
    Euler.z = atan2(siny_cosp, cosy_cosp);

    return Euler;
}

FMatrix FQuat::ToMatrix() const
{
    FMatrix M;

    float xx = x * x;
    float yy = y * y;
    float zz = z * z;

    float xy = x * y;
    float yz = y * z;
    float zx = z * x;

    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    M.m[0][0] = 1.0f - 2.0f * (yy + zz);
    M.m[0][1] = 2.0f * (xy + wz);
    M.m[0][2] = 2.0f * (zx - wy);

    M.m[1][0] = 2.0f * (xy - wz);
    M.m[1][1] = 1.0f - 2.0f * (xx + zz);
    M.m[1][2] = 2.0f * (yz + wx);

    M.m[2][0] = 2.0f * (zx + wy);
    M.m[2][1] = 2.0f * (yz - wx);
    M.m[2][2] = 1.0f - 2.0f * (xx + yy);
    return M;
}

FQuat FQuat::FromAxisAngle(const FVector& Axis, float Angle)
{
    FVector N = Axis;
    N.Normalize();

    float HalfAngle = Angle * 0.5f;

    float S = sin(HalfAngle);
    float C = cos(HalfAngle);

    return FQuat(N.x * S, N.y * S, N.z * S, C);
}

void FQuat::Normalize()
{
    float Length = sqrt(x * x + y * y + z * z + w * w);
    x /= Length;
    y /= Length;
    z /= Length;
    w /= Length;
}

FQuat FQuat::operator*(const FQuat& Other) const
{
    return FQuat(w * Other.x + x * Other.w + y * Other.z - z * Other.y,
                 w * Other.y - x * Other.z + y * Other.w + z * Other.x,
                 w * Other.z + x * Other.y - y * Other.x + z * Other.w,
                 w * Other.w - x * Other.x - y * Other.y - z * Other.z);
}

FVector FQuat::RotateVector(const FVector& v) const
{
    FQuat qv(v.x, v.y, v.z, 0.0f);

    FQuat result = (*this) * qv * Inverse();

    return FVector(result.x, result.y, result.z);
}

FQuat FQuat::Inverse() const
{
    float SizeSquared = x * x + y * y + z * z + w * w;

    if (SizeSquared == 0.0f)
        return FQuat(0, 0, 0, 1);

    return FQuat(-x / SizeSquared, -y / SizeSquared, -z / SizeSquared, w / SizeSquared);
}