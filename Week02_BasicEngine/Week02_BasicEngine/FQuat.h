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

    FMatrix ToMatrix() const;

    FQuat FromAxisAngle(const FVector& Axis, float Angle);

    void Normalize();

    FQuat operator*(const FQuat& Other) const;
};