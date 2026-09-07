#include "FMatrix.h"
#include "Utils/Math.h"
#include "FVector.h"
#include <algorithm>
#include <cmath>

const FMatrix FMatrix::Identity{};

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

FMatrix FMatrix::operator+(const FMatrix& other) const
{
	FMatrix result;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			result.m[i][j] = 0.0f;
			for (int k = 0; k < 4; ++k)
			{
				result.m[i][j] = m[i][k] + other.m[k][j];
			}
		}
	}
	return result;
}

FMatrix FMatrix::operator-(const FMatrix& other) const
{
	FMatrix result;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			result.m[i][j] = 0.0f;
			for (int k = 0; k < 4; ++k)
			{
				result.m[i][j] = m[i][k] - other.m[k][j];
			}
		}
	}
	return result;
}

FMatrix FMatrix::operator*(float scalar) const
{
	FMatrix result;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			result.m[i][j] = this->m[i][j];
			for (int k = 0; k < 4; ++k)
			{
				result.m[i][j] *= scalar;
			}
		}
	}
	return result;
}

FMatrix FMatrix::operator/(float scalar) const
{
	if (scalar < KINDA_SMALL_NUMBER)	return (*this);
	FMatrix result;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			result.m[i][j] = this->m[i][j];
			for (int k = 0; k < 4; ++k)
			{
				result.m[i][j] /= scalar;
			}
		}
	}
	return result;
}

FMatrix FMatrix::Translation(FVector location)
{
	FMatrix result;

	result.m[3][0] = location.x;
	result.m[3][1] = location.y;
	result.m[3][2] = location.z;

	return result;
}

FMatrix FMatrix::Scaling(FVector scale)
{
	FMatrix result;

	result.m[0][0] = scale.x;
	result.m[1][1] = scale.y;
	result.m[2][2] = scale.z;

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

FMatrix FMatrix::Rotation(FVector rotation)
{
	FMatrix rotationX = RotationX(rotation.x);
	FMatrix rotationY = RotationY(rotation.y);
	FMatrix rotationZ = RotationZ(rotation.z);
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

FMatrix FMatrix::MatrixInverse(const FMatrix& src)
{

	FMatrix tmp;
	float det[4];
	FMatrix M;

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
		M = FMatrix::Identity;
		return M;
	}
	const float RDet = 1.0f / determinant;

	M.m[0][0] = RDet * det[0];
	M.m[0][1] = -RDet * det[1];
	M.m[0][2] = RDet * det[2];
	M.m[0][3] = -RDet * det[3];
	M.m[1][0] = -RDet * (src.m[1][0] * tmp.m[0][0] - src.m[2][0] * tmp.m[0][1] + src.m[3][0] * tmp.m[0][2]);
	M.m[1][1] = RDet * (src.m[0][0] * tmp.m[1][0] - src.m[2][0] * tmp.m[1][1] + src.m[3][0] * tmp.m[1][2]);
	M.m[1][2] = -RDet * (src.m[0][0] * tmp.m[2][0] - src.m[1][0] * tmp.m[2][1] + src.m[3][0] * tmp.m[2][2]);
	M.m[1][3] = RDet * (src.m[0][0] * tmp.m[3][0] - src.m[1][0] * tmp.m[3][1] + src.m[2][0] * tmp.m[3][2]);
	M.m[2][0] = RDet * (
		src.m[1][0] * (src.m[2][1] * src.m[3][3] - src.m[2][3] * src.m[3][1]) -
		src.m[2][0] * (src.m[1][1] * src.m[3][3] - src.m[1][3] * src.m[3][1]) +
		src.m[3][0] * (src.m[1][1] * src.m[2][3] - src.m[1][3] * src.m[2][1])
		);
	M.m[2][1] = -RDet * (
		src.m[0][0] * (src.m[2][1] * src.m[3][3] - src.m[2][3] * src.m[3][1]) -
		src.m[2][0] * (src.m[0][1] * src.m[3][3] - src.m[0][3] * src.m[3][1]) +
		src.m[3][0] * (src.m[0][1] * src.m[2][3] - src.m[0][3] * src.m[2][1])
		);
	M.m[2][2] = RDet * (
		src.m[0][0] * (src.m[1][1] * src.m[3][3] - src.m[1][3] * src.m[3][1]) -
		src.m[1][0] * (src.m[0][1] * src.m[3][3] - src.m[0][3] * src.m[3][1]) +
		src.m[3][0] * (src.m[0][1] * src.m[1][3] - src.m[0][3] * src.m[1][1])
		);
	M.m[2][3] = -RDet * (
		src.m[0][0] * (src.m[1][1] * src.m[2][3] - src.m[1][3] * src.m[2][1]) -
		src.m[1][0] * (src.m[0][1] * src.m[2][3] - src.m[0][3] * src.m[2][1]) +
		src.m[2][0] * (src.m[0][1] * src.m[1][3] - src.m[0][3] * src.m[1][1])
		);
	M.m[3][0] = -RDet * (
		src.m[1][0] * (src.m[2][1] * src.m[3][2] - src.m[2][2] * src.m[3][1]) -
		src.m[2][0] * (src.m[1][1] * src.m[3][2] - src.m[1][2] * src.m[3][1]) +
		src.m[3][0] * (src.m[1][1] * src.m[2][2] - src.m[1][2] * src.m[2][1])
		);
	M.m[3][1] = RDet * (
		src.m[0][0] * (src.m[2][1] * src.m[3][2] - src.m[2][2] * src.m[3][1]) -
		src.m[2][0] * (src.m[0][1] * src.m[3][2] - src.m[0][2] * src.m[3][1]) +
		src.m[3][0] * (src.m[0][1] * src.m[2][2] - src.m[0][2] * src.m[2][1])
		);
	M.m[3][2] = -RDet * (
		src.m[0][0] * (src.m[1][1] * src.m[3][2] - src.m[1][2] * src.m[3][1]) -
		src.m[1][0] * (src.m[0][1] * src.m[3][2] - src.m[0][2] * src.m[3][1]) +
		src.m[3][0] * (src.m[0][1] * src.m[1][2] - src.m[0][2] * src.m[1][1])
		);
	M.m[3][3] = RDet * (
		src.m[0][0] * (src.m[1][1] * src.m[2][2] - src.m[1][2] * src.m[2][1]) -
		src.m[1][0] * (src.m[0][1] * src.m[2][2] - src.m[0][2] * src.m[2][1]) +
		src.m[2][0] * (src.m[0][1] * src.m[1][2] - src.m[0][2] * src.m[1][1])
		);


	return M;

}

bool FMatrix::IsOrthogonal()
{
	FVector X(m[0][0], m[0][1], m[0][1]);
	FVector Y(m[1][0], m[1][1], m[1][1]);
	FVector Z(m[2][0], m[2][1], m[2][1]);

	float XY = X.Dot(Y);
	float YZ = Y.Dot(Z);
	float ZX = Z.Dot(X);
	return Abs(XY) <= KINDA_SMALL_NUMBER && Abs(YZ) <= KINDA_SMALL_NUMBER && Abs(ZX) <= KINDA_SMALL_NUMBER;
}

bool FMatrix::IsOrthonormal()
{
	FVector X(m[0][0], m[0][1], m[0][2]);
	FVector Y(m[1][0], m[1][1], m[1][2]);
	FVector Z(m[2][0], m[2][1], m[2][2]);

	float XX = X.x * X.x + X.y * X.y + X.z * X.z;
	float YY = Y.x * Y.x + Y.y * Y.y + Y.z * Y.z;
	float ZZ = Z.x * Z.x + Z.y * Z.y + Z.z * Z.z;

	float XY = X.x * Y.x + X.y * Y.y + X.z * Y.z;
	float YZ = Y.x * Z.x + Y.y * Z.y + Y.z * Z.z;
	float ZX = Z.x * X.x + Z.y * X.y + Z.z * X.z;

	return Abs(XX - 1.0f) <= KINDA_SMALL_NUMBER && Abs(YY - 1.0f) <= KINDA_SMALL_NUMBER && Abs(ZZ - 1.0f) <= KINDA_SMALL_NUMBER &&
		Abs(XY) <= KINDA_SMALL_NUMBER && Abs(YZ) <= KINDA_SMALL_NUMBER && Abs(ZX) <= KINDA_SMALL_NUMBER;
}

FMatrix FMatrix::GetNormalMatrix()
{
	FMatrix Inverse;
	if (!MatrixInverse(*this, Inverse)) {
		return Identity;
	}
	return Inverse.Transposed();
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
	return  FMatrix::Scaling(Scale) *
		FMatrix::Rotation(Rotation) *
		FMatrix::Translation(Location);

}


static FMatrix GetModelMatrixInverse(const FVector& Location, const FVector& Rotation, const FVector& Scale)
{
	return FMatrix::TranslationMatrixInverse(Location) *
		FMatrix::RotationMatrixInverse(Rotation) *
		FMatrix::ScaleMatrixInverse(Scale);
}

FMatrix FMatrix::GetViewMatrix(const FVector& Location, const FVector& Rotation) {
	FVector ZAxis(cos(Rotation.y) * cos(Rotation.x), sin(Rotation.x), -sin(Rotation.y) * cos(Rotation.x));
	ZAxis.Normalize();
	FVector XAxis = FVector(0, 1, 0).Cross(ZAxis);
	XAxis.Normalize();
	FVector YAxis = ZAxis.Cross(XAxis);
	YAxis.Normalize();
	FMatrix M;
	M.m[0][0] = XAxis.x;
	M.m[0][1] = YAxis.x;
	M.m[0][2] = ZAxis.x;
	M.m[0][3] = 0.0f;
	M.m[1][0] = XAxis.y;
	M.m[1][1] = YAxis.y;
	M.m[1][2] = ZAxis.y;
	M.m[1][3] = 0.0f;
	M.m[2][0] = XAxis.z;
	M.m[2][1] = YAxis.z;
	M.m[2][2] = ZAxis.z;
	M.m[2][3] = 0.0f;
	M.m[3][0] = -Location.x * XAxis.x - Location.y * XAxis.y - Location.z * XAxis.z;
	M.m[3][1] = -Location.x * YAxis.x - Location.y * YAxis.y - Location.z * YAxis.z;
	M.m[3][2] = -Location.x * ZAxis.x - Location.y * ZAxis.y - Location.z * ZAxis.z;
	M.m[3][3] = 1.0f;
	return M;
}

FMatrix FMatrix::GetViewMatrixInverse(const FVector& Location, const FVector& Rotation) {
	return FMatrix::MatrixInverse(FMatrix::GetViewMatrix(Location, Rotation) );
}

FMatrix FMatrix::GetProjectionMatrix(float FOV, float AspectRatio, float NearClip, float FarClip, bool IsOrthogonal) {
	if (IsOrthogonal)
		return Orthographic(FOV, AspectRatio, NearClip, FarClip);
	else
		return Perspective(FOV, AspectRatio, NearClip, FarClip);
}

FMatrix FMatrix::GetProjectionMatrixInverse(float FOV, float AspectRatio, float NearClip, float FarClip, bool IsOrthogonal) {
	if (IsOrthogonal)
		return  FMatrix::MatrixInverse(Orthographic(FOV, AspectRatio, NearClip, FarClip));
	else
		return  FMatrix::MatrixInverse(Perspective(FOV, AspectRatio, NearClip, FarClip));
}

FMatrix FMatrix::Perspective(float FOV, float AspectRatio, float NearClip, float FarClip)
{
	FMatrix M;
	M.m[0][0] = 1 / (tan(FOV) * AspectRatio);
	M.m[1][1] = 1 / tan(FOV);
	M.m[2][2] = (FarClip) / (FarClip - NearClip);
	M.m[2][3] = 1;
	M.m[3][2] = -(FarClip * NearClip) / (FarClip - NearClip);
	M.m[3][3] = 0;
	return M;
}

FMatrix FMatrix::Orthographic(float FOV, float AspectRatio, float NearClip, float FarClip)
{
	const float Height = 2.0f;
	const float Width = Height * AspectRatio;
	FMatrix M;
	M.m[0][0] = 2.0f / Width;
	M.m[1][1] = 2.0f / Height;
	M.m[2][2] = 1 / (FarClip - NearClip);
	M.m[3][2] = -NearClip / (FarClip - NearClip);
	return M;
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

