#pragma once

#include "Utils/Math.h"
#include "FString.h"

struct FVector
{
public:
    float x, y, z;
    FVector(float _x = 0, float _y = 0, float _z = 0) : x(_x), y(_y), z(_z) {}
       

    [[nodiscard]] __forceinline FVector operator+(const FVector& V) const
    {
        return FVector(x + V.x, y + V.y, z + V.z);
    }

    [[nodiscard]] __forceinline FVector operator-(const FVector& V) const
    {
        return FVector(x - V.x, y - V.y, z - V.z);
    }

    [[nodiscard]] __forceinline FVector operator*(const FVector& V) const
    {
        return FVector(x * V.x, y * V.y, z * V.z);
    }

    [[nodiscard]] __forceinline FVector operator/(const FVector& V) const
    {
        return FVector(x / V.x, y / V.y, z / V.z);
    }

    [[nodiscard]] __forceinline bool operator==(const FVector& V) const
    {
        return x == V.x && y == V.y && z == V.z;
    }

    [[nodiscard]] __forceinline bool operator!=(const FVector& V) const
    {
        return x != V.x || y != V.y || z != V.z;
    }

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
    [[nodiscard]] FVector Cross(const FVector& v1, const FVector& v2) const
    {
        FVector cross;
        cross.x = v1.y * v2.z - v1.z * v2.y;
        cross.y = v1.z * v2.x - v1.x * v2.z;
        cross.z = v1.x * v2.y - v1.y * v2.x;

        return cross;
    }

    [[nodiscard]] FVector Cross(const FVector& v1)const
    {
        return Cross(*this, v1);
    }

    [[nodiscard]] float Dot(const FVector& v1, const FVector& v2) const
    {
        float dot = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

        return dot;
    }

    [[nodiscard]] float Dot(const FVector& v1) const
    {
        return Dot(*this, v1);
    }

    [[nodiscard]] __forceinline bool Equals(const FVector& V, float Tolerance = KINDA_SMALL_NUMBER) const;

    [[nodiscard]] float Size() const;

    [[nodiscard]] float SizeSquared() const;

    [[nodiscard]] float Length() const;

    [[nodiscard]] float SquaredLength() const;

    [[nodiscard]] bool IsNearlyZero(float Tolerance = KINDA_SMALL_NUMBER) const;

    [[nodiscard]] bool IsZero() const;

    [[nodiscard]] FString ToString() const;

    FVector RotationFromAxes(
        const FVector& XAxis,
        const FVector& YAxis,
        const FVector& ZAxis)
    {
        FMatrix R;

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

struct FVector2
{
public:
    float x, y;

    [[nodiscard]] FVector2() = default;
    [[nodiscard]] FVector2(float _x = 0, float _y = 0) : x(_x), y(_y) {}

    [[nodiscard]] __forceinline FVector2 operator+(const FVector2& V) const
    {
        return FVector2(x + V.x, y + V.y);
    }

    [[nodiscard]] __forceinline FVector2 operator-(const FVector2& V) const
    {
        return FVector2(x - V.x, y - V.y);
    }

    [[nodiscard]] __forceinline FVector2 operator*(const FVector2& V) const
    {
        return FVector2(x * V.x, y * V.y);
    }

    [[nodiscard]] __forceinline FVector2 operator/(const FVector2& V) const
    {
        return FVector2(x / V.x, y / V.y);
    }

    [[nodiscard]] __forceinline bool operator==(const FVector2& V) const
    {
        return x == V.x && y == V.y;
    }

    [[nodiscard]] __forceinline bool operator!=(const FVector2& V) const
    {
        return x != V.x || y != V.y;
    }

    void Normalize()
    {
        float d = x * x + y * y;
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
    }

    [[nodiscard]] float Dot(const FVector2& v1, const FVector2& v2) const
    {
        float dot = v1.x * v2.x + v1.y * v2.y;

        return dot;
    }

    [[nodiscard]] float Dot(const FVector2& v1) const
    {
        return Dot(*this, v1);
    }

    [[nodiscard]] __forceinline bool Equals(const FVector2& V, float Tolerance = KINDA_SMALL_NUMBER) const;

    [[nodiscard]] float Size() const;

    [[nodiscard]] float SizeSquared() const;

    [[nodiscard]] float Length() const;

    [[nodiscard]] float SquaredLength() const;

    [[nodiscard]] bool IsNearlyZero(float Tolerance = KINDA_SMALL_NUMBER) const;

    [[nodiscard]] bool IsZero() const;

    [[nodiscard]] FString ToString() const;
};



