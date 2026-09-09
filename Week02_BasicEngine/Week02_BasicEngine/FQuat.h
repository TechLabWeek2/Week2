#pragma once

struct FVector;
struct FMatrix;
struct FQuat
{
    float x;
    float y;
    float z;
    float w;

    FQuat() : x(0), y(0), z(0), w(1) {
    }

    FQuat(float X, float Y, float Z, float W) : x(X), y(Y), z(Z), w(W) {
    }

    FQuat FromEuler(const FVector& Euler);
    FVector ToEuler(const FQuat& Q) const;
    FMatrix ToMatrix() const;

    FQuat FromAxisAngle(const FVector& Axis, float Angle);

    void Normalize();

    FQuat operator*(const FQuat& Other) const;
    FVector RotateVector(const FVector& v) const;
    FQuat Inverse() const;
    FQuat Concatenate(const FQuat& Q1, const FQuat& Q2) {
        FQuat Result;

        Result.x = Q1.w * Q2.x + Q1.x * Q2.w + Q1.y * Q2.z - Q1.z * Q2.y;
        Result.y = Q1.w * Q2.y - Q1.x * Q2.z + Q1.y * Q2.w + Q1.z * Q2.x;
        Result.z = Q1.w * Q2.z + Q1.x * Q2.y - Q1.y * Q2.x + Q1.z * Q2.w;
        Result.w = Q1.w * Q2.w - Q1.x * Q2.x - Q1.y * Q2.y - Q1.z * Q2.z;

        return Result;
    }
};