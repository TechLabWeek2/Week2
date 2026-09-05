#pragma once 
#include <math.h>
#include <algorithm>
#include <cmath>


constexpr float PI = 3.14159265358979323846f;

inline float DegreeToRadian(float degree)
{
	return degree * PI / 180.0f;
}

struct Matrix3x3
{
public:
	Matrix3x3()
	{
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				m[i][j] = (i == j) ? 1.0f : 0.0f;
			}
		}
		
	}
	
	~Matrix3x3() = default;

	Matrix3x3(const Matrix3x3&) = default;
	Matrix3x3& operator=(const Matrix3x3&) = default;

	Matrix3x3(Matrix3x3&&)= default;
	Matrix3x3& operator=(Matrix3x3&&) = default;

	Matrix3x3 operator*(const Matrix3x3& other) const
	{
		Matrix3x3 result;
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

public:

	static Matrix3x3 Orthographic(float left, float right, float bottom, float top)
	{
		Matrix3x3 result;
		result.m[0][0] = 2.0f / (right - left);
		result.m[1][1] = 2.0f / (top - bottom);
		result.m[0][2] = -(right + left) / (right - left);
		result.m[1][2] = -(top + bottom) / (top - bottom);
		result.m[2][2] = 1.0f;

		return result;
	}

	static Matrix3x3 Translation(float tx, float ty)
	{
		Matrix3x3 result;
		result.m[0][2] = tx;
		result.m[1][2] = ty;
		return result;
	}

	static Matrix3x3 Rotation(float angle)
	{
		Matrix3x3 result;
		float cosA = std::cos(angle);
		float sinA = std::sin(angle);
		result.m[0][0] = cosA;
		result.m[0][1] = -sinA;
		result.m[1][0] = sinA;
		result.m[1][1] = cosA;
		return result;
	}


public:

	void Transpose()
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = i + 1; j < 4; ++j)
			{
				std::swap(m[i][j], m[j][i]);
			}
		}
	}

public:
	float m[4][4];
};

struct FMatrix4x4
{
	public:
		FMatrix4x4()
		{
			for (int i = 0; i < 4; ++i) {
				for (int j = 0; j < 4; ++j) {
					m[i][j] = (i == j) ? 1.0f : 0.0f;
				}
			}

		}

		~FMatrix4x4() = default;

		FMatrix4x4(const FMatrix4x4&) = default;
		FMatrix4x4& operator=(const FMatrix4x4&) = default;

		FMatrix4x4(FMatrix4x4&&) = default;
		FMatrix4x4& operator=(FMatrix4x4&&) = default;

		FMatrix4x4 operator*(const FMatrix4x4& other) const
		{
			FMatrix4x4 result;
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
	public:

		// 이동 행렬
		static FMatrix4x4 Translation(float x, float y, float z)
		{
			FMatrix4x4 result;

			result.m[3][0] = x;
			result.m[3][1] = y;
			result.m[3][2] = z;

			return result;
		}

		static FMatrix4x4 Scaling(float sx, float sy, float sz)
		{
			FMatrix4x4 result;

			result.m[0][0] = sx;
			result.m[1][1] = sy;
			result.m[2][2] = sz;

			return result;
		}
		//angle은 라디안 단위
		static FMatrix4x4 RotationX(float angle)
		{
			FMatrix4x4 result;
			
			result.m[1][1] = std::cos(angle);
			result.m[2][2] = std::cos(angle);
			result.m[1][2] = std::sin(angle);
			result.m[2][1] = -std::sin(angle);
			return result;
		}

		static FMatrix4x4 RotationY(float angle)
		{
			FMatrix4x4 result;
			result.m[0][0] = std::cos(angle);
			result.m[0][2] = -std::sin(angle);
			result.m[2][0] = std::sin(angle);
			result.m[2][2] = std::cos(angle);
			return result;
		}

		static FMatrix4x4 RotationZ(float angle)
		{
			FMatrix4x4 result;
			result.m[0][0] = std::cos(angle);
			result.m[0][1] = std::sin(angle);
			result.m[1][0] = -std::sin(angle);
			result.m[1][1] = std::cos(angle);
			return result;
		}

		//입력은 degree 단위
		static FMatrix4x4 Rotation(float angleX, float angleY, float angleZ)
		{
			FMatrix4x4 rotationX = RotationX(DegreeToRadian(angleX));
			FMatrix4x4 rotationY = RotationY(DegreeToRadian(angleY));
			FMatrix4x4 rotationZ = RotationZ(DegreeToRadian(angleZ));
			return rotationZ * rotationY * rotationX;
		}	

		// 역행렬을 계산하고 out에 결과를 저장, 역행렬을 구할 수 없다면 identity 행렬을 반환하고 false를 반환
		static bool MatrixInverse(const FMatrix4x4& src, FMatrix4x4& out)
		{
			FMatrix4x4 tmp;
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
				out = FMatrix4x4::Identity;
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
		
		// Transpose된 행렬을 반환
		FMatrix4x4 Transposed() const
		{
			FMatrix4x4 result;

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					result.m[i][j] = m[j][i];
				}
			}

			return result;
		}

		//현재 자기 자신을 직접 Transpose
		void Transpose()
		{
			for (int i = 0; i < 4; ++i)
			{
				for (int j = i + 1; j < 4; ++j)
				{
					std::swap(m[i][j], m[j][i]);
				}
			}
		}
		
		
	public:
		float m[4][4];
		static const FMatrix4x4 Identity;

};

inline const FMatrix4x4 FMatrix4x4::Identity{};