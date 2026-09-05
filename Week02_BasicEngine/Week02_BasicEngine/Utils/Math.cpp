#include "Math.h"
#include <FVector.h>
#include <algorithm>
#include <cmath>

const FMatrix FMatrix::Identity{};

float DegreeToRadian(float degree)
{
	return degree * PI / 180.0f;
}

FMatrix::FMatrix(
	float m00, float m01, float m02, float m03,
	float m10, float m11, float m12, float m13,
	float m20, float m21, float m22, float m23,
	float m30, float m31, float m32, float m33)
{
	m[0][0] = m00; m[0][1] = m01; m[0][2] = m02; m[0][3] = m03;
	m[1][0] = m10; m[1][1] = m11; m[1][2] = m12; m[1][3] = m13;
	m[2][0] = m20; m[2][1] = m21; m[2][2] = m22; m[2][3] = m23;
	m[3][0] = m30; m[3][1] = m31; m[3][2] = m32; m[3][3] = m33;
}


FMatrix::FMatrix()
{
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			m[i][j] = (i == j) ? 1.0f : 0.0f;
		}
	}

}

FMatrix FMatrix::operator*(const FMatrix& other) const
{
	FMatrix result;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			result.m[i][j] = 0.0f;
			for (int k = 0; k < 4; ++k)
			{
				result.m[i][j] += m[i][k] * other.m[k][j];
			}
		}
	}
	return result;
}

FMatrix FMatrix::Translation(float x, float y, float z)
{
	FMatrix result;

	result.m[3][0] = x;
	result.m[3][1] = y;
	result.m[3][2] = z;

	return result;
}

FMatrix FMatrix::Scaling(float sx, float sy, float sz)
{
	FMatrix result;

	result.m[0][0] = sx;
	result.m[1][1] = sy;
	result.m[2][2] = sz;

	return result;
}

FMatrix FMatrix::RotationX(float angle)
{
	FMatrix result;
	
	result.m[1][1] = std::cos(angle);
	result.m[2][2] = std::cos(angle);
	result.m[1][2] = std::sin(angle);
	result.m[2][1] = -std::sin(angle);
	return result;
}

FMatrix FMatrix::RotationY(float angle)
{
	FMatrix result;
	result.m[0][0] = std::cos(angle);
	result.m[0][2] = -std::sin(angle);
	result.m[2][0] = std::sin(angle);
	result.m[2][2] = std::cos(angle);
	return result;
}

FMatrix FMatrix::RotationZ(float angle)
{
	FMatrix result;
	result.m[0][0] = std::cos(angle);
	result.m[0][1] = std::sin(angle);
	result.m[1][0] = -std::sin(angle);
	result.m[1][1] = std::cos(angle);
	return result;
}

FMatrix FMatrix::Rotation(float angleX, float angleY, float angleZ)
{
	FMatrix rotationX = RotationX(DegreeToRadian(angleX));
	FMatrix rotationY = RotationY(DegreeToRadian(angleY));
	FMatrix rotationZ = RotationZ(DegreeToRadian(angleZ));
	return rotationZ * rotationY * rotationX;
}


bool FMatrix::MatrixInverse(const FMatrix& src, FMatrix& out)
{
	FMatrix tmp;
	float det[4];

	//2x2 minor determinant를 미리 계산
	tmp.m[0][0] = src.m[2][2] * src.m[3][3] - src.m[2][3] * src.m[3][2]; 
	tmp.m[0][1] = src.m[1][2] * src.m[3][3] - src.m[1][3] * src.m[3][2];
	tmp.m[0][2] = src.m[1][2] * src.m[2][3] - src.m[1][3] * src.m[2][2];
				  
	tmp.m[1][0] = src.m[2][2] * src.m[3][3] - src.m[2][3] * src.m[3][2];
	tmp.m[1][1] = src.m[0][2] * src.m[3][3] - src.m[0][3] * src.m[3][2];
	tmp.m[1][2] = src.m[0][2] * src.m[2][3] - src.m[0][3] * src.m[2][2];
				 
	tmp.m[2][0] = src.m[1][2] * src.m[3][3] - src.m[1][3] * src.m[3][2];
	tmp.m[2][1] = src.m[0][2] * src.m[3][3] - src.m[0][3] * src.m[3][2];
	tmp.m[2][2] = src.m[0][2] * src.m[1][3] - src.m[0][3] * src.m[1][2];
				  
	tmp.m[3][0] = src.m[1][2] * src.m[2][3] - src.m[1][3] * src.m[2][2];
	tmp.m[3][1] = src.m[0][2] * src.m[2][3] - src.m[0][3] * src.m[2][2];
	tmp.m[3][2] = src.m[0][2] * src.m[1][3] - src.m[0][3] * src.m[1][2];
	
	//3x3 minor determinant 계산
	det[0] = src.m[1][1] * tmp.m[0][0] - src.m[2][1] * tmp.m[0][1] + src.m[3][1] * tmp.m[0][2];
	det[1] = src.m[0][1] * tmp.m[1][0] - src.m[2][1] * tmp.m[1][1] + src.m[3][1] * tmp.m[1][2];
	det[2] = src.m[0][1] * tmp.m[2][0] - src.m[1][1] * tmp.m[2][1] + src.m[3][1] * tmp.m[2][2];
	det[3] = src.m[0][1] * tmp.m[3][0] - src.m[1][1] * tmp.m[3][1] + src.m[2][1] * tmp.m[3][2];

	// Determinant 계산
	const float determinant = src.m[0][0] * det[0] - src.m[1][0] * det[1] + src.m[2][0] * det[2] - src.m[3][0] * det[3];

	if (determinant == 0.0f)
	{
		out = FMatrix::Identity;
		return false;
	}

	const float RDet = 1.0f / determinant;

	out.m[0][0] = RDet * det[0];
	out.m[0][1] = -RDet * det[1];
	out.m[0][2] = RDet * det[2];
	out.m[0][3] = -RDet * det[3];
	out.m[1][0] = -RDet * (src.m[1][0] * tmp.m[0][0] - src.m[2][0] * tmp.m[0][1] + src.m[3][0] * tmp.m[0][2]);
	out.m[1][1] = RDet * (src.m[0][0] * tmp.m[1][0] - src.m[2][0] * tmp.m[1][1] + src.m[3][0] * tmp.m[1][2]);
	out.m[1][2] = -RDet * (src.m[0][0] * tmp.m[2][0] - src.m[1][0] * tmp.m[2][1] + src.m[3][0] * tmp.m[2][2]);
	out.m[1][3] = RDet * (src.m[0][0] * tmp.m[3][0] - src.m[1][0] * tmp.m[3][1] + src.m[2][0] * tmp.m[3][2]);
	out.m[2][0] = RDet * (
		src.m[1][0] * (src.m[2][1] * src.m[3][3] - src.m[2][3] * src.m[3][1]) -
		src.m[2][0] * (src.m[1][1] * src.m[3][3] - src.m[1][3] * src.m[3][1]) +
		src.m[3][0] * (src.m[1][1] * src.m[2][3] - src.m[1][3] * src.m[2][1])
		);
	out.m[2][1] = -RDet * (
		src.m[0][0] * (src.m[2][1] * src.m[3][3] - src.m[2][3] * src.m[3][1]) -
		src.m[2][0] * (src.m[0][1] * src.m[3][3] - src.m[0][3] * src.m[3][1]) +
		src.m[3][0] * (src.m[0][1] * src.m[2][3] - src.m[0][3] * src.m[2][1])
		);
	out.m[2][2] = RDet * (
		src.m[0][0] * (src.m[1][1] * src.m[3][3] - src.m[1][3] * src.m[3][1]) -
		src.m[1][0] * (src.m[0][1] * src.m[3][3] - src.m[0][3] * src.m[3][1]) +
		src.m[3][0] * (src.m[0][1] * src.m[1][3] - src.m[0][3] * src.m[1][1])
		);
	out.m[2][3] = -RDet * (
		src.m[0][0] * (src.m[1][1] * src.m[2][3] - src.m[1][3] * src.m[2][1]) -
		src.m[1][0] * (src.m[0][1] * src.m[2][3] - src.m[0][3] * src.m[2][1]) +
		src.m[2][0] * (src.m[0][1] * src.m[1][3] - src.m[0][3] * src.m[1][1])
		);
	out.m[3][0] = -RDet * (
		src.m[1][0] * (src.m[2][1] * src.m[3][2] - src.m[2][2] * src.m[3][1]) -
		src.m[2][0] * (src.m[1][1] * src.m[3][2] - src.m[1][2] * src.m[3][1]) +
		src.m[3][0] * (src.m[1][1] * src.m[2][2] - src.m[1][2] * src.m[2][1])
		);
	out.m[3][1] = RDet * (
		src.m[0][0] * (src.m[2][1] * src.m[3][2] - src.m[2][2] * src.m[3][1]) -
		src.m[2][0] * (src.m[0][1] * src.m[3][2] - src.m[0][2] * src.m[3][1]) +
		src.m[3][0] * (src.m[0][1] * src.m[2][2] - src.m[0][2] * src.m[2][1])
		);
	out.m[3][2] = -RDet * (
		src.m[0][0] * (src.m[1][1] * src.m[3][2] - src.m[1][2] * src.m[3][1]) -
		src.m[1][0] * (src.m[0][1] * src.m[3][2] - src.m[0][2] * src.m[3][1]) +
		src.m[3][0] * (src.m[0][1] * src.m[1][2] - src.m[0][2] * src.m[1][1])
		);
	out.m[3][3] = RDet * (
		src.m[0][0] * (src.m[1][1] * src.m[2][2] - src.m[1][2] * src.m[2][1]) -
		src.m[1][0] * (src.m[0][1] * src.m[2][2] - src.m[0][2] * src.m[2][1]) +
		src.m[2][0] * (src.m[0][1] * src.m[1][2] - src.m[0][2] * src.m[1][1])
		);


	return true;

}

FMatrix FMatrix::Transposed() const
{
	FMatrix result;

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			result.m[i][j] = m[j][i];
		}
	}

	return result;
}

void FMatrix::Transpose()
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = i + 1; j < 4; ++j)
		{
			std::swap(m[i][j], m[j][i]);
		}
	}
}


static FMatrix GetModelMatrix(const FVector& Location, const FVector& Rotation, const FVector& Scale)
{
	return  FMatrix::Scaling(Scale.x, Scale.y, Scale.z) *
		FMatrix::Rotation(Rotation.x, Rotation.y, Rotation.z) *
		FMatrix::Translation(Location.x, Location.y, Location.z);
		
}


static FMatrix GetModelMatrixInverse(const FVector& Location, const FVector& Rotation, const FVector& Scale)
{ 
	return FMatrix::TranslationMatrixInverse(Location) *
		FMatrix::RotationMatrixInverse(Rotation) *
		FMatrix::ScaleMatrixInverse(Scale);
}


static FMatrix TranslationMatrixInverse(const FVector& Other)
{
	FMatrix result;
	result.m[3][0] = -Other.x;
	result.m[3][1] = -Other.y;
	result.m[3][2] = -Other.z;
	result.m[3][3] = 1.0f;

	return result;
}

static FMatrix ScaleMatrixInverse(const FVector& Other)
{
	FMatrix result;
	result.m[0][0] = 1.0f / Other.x;
	result.m[1][1] = 1.0f / Other.y;
	result.m[2][2] = 1.0f / Other.z;
	result.m[3][3] = 1.0f;

	return result;
}

static FMatrix RotationMatrixInverse(const FVector& Other)
{
	return FMatrix::RotationX(-DegreeToRadian(Other.x)) *
		FMatrix::RotationY(-DegreeToRadian(Other.y)) *
		FMatrix::RotationZ(-DegreeToRadian(Other.z));
}

