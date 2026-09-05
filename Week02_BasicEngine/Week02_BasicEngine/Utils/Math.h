#pragma once 


#define KINDA_SMALL_NUMBER	(1.e-4f)

constexpr float PI = 3.14159265358979323846f;

struct FVector;

template<typename T>
inline T Abs(const T A)
{
	return (A < (T)0) ? (T)-A : A;
}

float DegreeToRadian(float degree);


struct FMatrix
{
public:
	FMatrix(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33);

	FMatrix();

	~FMatrix() = default;

	FMatrix(const FMatrix&) = default;
	FMatrix& operator=(const FMatrix&) = default;

	FMatrix(FMatrix&&) = default;
	FMatrix& operator=(FMatrix&&) = default;

	FMatrix operator*(const FMatrix& other) const;
public:

	
	// Rotation의 정보를 행렬로 변환하여 제공하는 함수
	static FMatrix RotationMatrix(const FVector& InOtherVector);
	static FMatrix RotationMatrixInverse(const FVector& InOtherVector);

/*	// Quaternion 기반 회전행렬 (row-major)
	static FMatrix RotationMatrix(const struct FQuat& Q);
	static FMatrix RotationMatrixInverse(const struct FQuat& Q);*/


	// 이동 행렬
	static FMatrix Translation(float x, float y, float z);
	static FMatrix TranslationMatrixInverse(const FVector& Other);


	static FMatrix Scaling(float sx, float sy, float sz);
	static FMatrix ScaleMatrixInverse(const FVector& Other);

	//angle은 라디안 단위
	static FMatrix RotationX(float angle);
				   
	static FMatrix RotationY(float angle);
				   
	static FMatrix RotationZ(float angle);

	//입력은 degree 단위
	static FMatrix Rotation(float angleX, float angleY, float angleZ);

	// 역행렬을 계산하고 out에 결과를 저장, 역행렬을 구할 수 없다면 identity 행렬을 반환하고 false를 반환
	static bool MatrixInverse(const FMatrix& src, FMatrix& out);

	// Transpose된 행렬을 반환
	FMatrix Transposed() const;

	//현재 자기 자신을 직접 Transpose
	void Transpose();


	static FMatrix GetModelMatrix(const FVector& Location, const FVector& Rotation, const FVector& Scale);

	static FMatrix GetModelMatrixInverse(const FVector& Location, const FVector& Rotation, const FVector& Scale);

	static FMatrix Perspective(float fovY, float aspect, float nearZ, float farZ);

	static FMatrix Orthographic(float fovY, float aspect, float nearZ, float farZ);

	/**
	 * @brief LHY+ -> UE(LHZ+, X-forward) 기준변환 행렬과 그 역행렬
	 * (x,y,z) -> (z,x,y) 순열 전환. 직교행렬이므로 역행렬은 전치행렬과 동일.
	 */
	static FMatrix BasisLHYToUE();
	static FMatrix BasisUEToLHY();

public:
	float m[4][4];
	static const FMatrix Identity;

};
