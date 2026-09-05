#pragma once

#include "Utils/Math.hpp"

class FVector
{
public:
    float x, y, z;
    FVector(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
    void Normalize()
    {
        float d = x * x + y * y + z * z;
        if (d == 1)
        {
            return;
        }
        if (d < (1.0E-8F))
        {
            return;
        }
        d = sqrt(d);
        x = x / d;
        y = y / d;
        z = z / d;
    }
    FVector Cross(FVector v1, FVector v2)
    {
        FVector cross;
        cross.x = v1.y * v2.z - v1.z * v2.y;
        cross.y = v1.z * v2.x - v1.x * v2.z;
        cross.z = v1.x * v2.y - v1.y * v2.x;

        return cross;
    }
    float Dot(FVector v1, FVector v2)
    {
        float dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

        return dot;
    }
    FVector RotationFromAxes(
        const FVector& XAxis,
        const FVector& YAxis,
        const FVector& ZAxis)
    {
        FMatrix4x4 R;

        R.m[0][0] = XAxis.x;
        R.m[0][1] = XAxis.y;
        R.m[0][2] = XAxis.z;

        R.m[1][0] = YAxis.x;
        R.m[1][1] = YAxis.y;
        R.m[1][2] = YAxis.z;

        R.m[2][0] = ZAxis.x;
        R.m[2][1] = ZAxis.y;
        R.m[2][2] = ZAxis.z;

        float angleY = std::asin(-R.m[2][0]);

        float angleX = std::atan2(
            R.m[2][1],
            R.m[2][2]
        );

        float angleZ = std::atan2(
            R.m[1][0],
            R.m[0][0]
        );

        return {
            (angleX * 57.3f),
            (angleY * 57.3f),
            (angleZ * 57.3f)
        };
    }
};

