#pragma once
#include "CoreType.h"
#include "TArray.h"
#include <vector>
#include <cassert>
#include <cstring>
#include <ostream>

//여러 문자열 타입을 대응하기 위해 쓰지만
// 나중에 확장하도록 하고 일단 하나만 둔다.
// 이해도 잘 안됨.
#define UE_STRING_CHARTYPE      TCHAR

//=======================================================
// FString 구현
// 참고 : UnrealString.h.inl
// 
// UE는 TArray<TCHAR>를 쓰지만 TArray가 만들어지기 전까지 
// std::vector<TCHAR>를 사용합니다.
// 나중에 자체 TArray가 만들어지면 DataType만 바꿉니다.
//==========================================================

class FString
{
public:
	//완전히 이해하지 못했지만 위의 여러 타입의 문자열에 대응하기 위한
	//매크로와 같이 다른 타입이 적용될 때 ElementType으로 typedef를 두면
	//타입을 검사할 때와 같은 경우 쓸 수 있다고 한다.
	//좀 더 이해가 필요하다.
	using ElementType = UE_STRING_CHARTYPE;

	FString() = default;
	FString(const ElementType* InStr);
	FString(const FString& Other) = default;		//복사 생성자
	FString(FString&& Other) noexcept = default;	//이동 생성자
	~FString() = default;

private:
	//문자열 저장
	//나중에 TArray가 생기면 여기만 바꾸면 됨.
	typedef TArray<ElementType> DataType;
	//typedef std::vector<ElementType> DataType;

	DataType Data;

public:
	//문자열 길이
	[[nodiscard]] __forceinline int32 Len()const
	{
		return Data.Num() ? Data.Num() - 1 : 0;
	}

	//비어 있는지
	bool IsEmpty() const
	{
		//널 종단문자 \0도 원소로 카운트한다
	// 그래서 0이면 완전히 빈 배열, 1이면 널 문자 하나만 있는 상태
		return Data.Num() <= 1;
	}

	//포인터 접근
	[[nodiscard]] __forceinline const ElementType* operator*() const
	{
		//data() : 내부 버퍼의 raw point 반환 메서드
		return Data.IsEmpty() ? L"" : Data.Data();
	}

	inline ElementType& operator[](int32 Index)
	{
		assert((IsValidIndex(Index), "String index out of bounds: Index %i from a string with a length of %i", Index, Len()));
		return Data[Index];
	}

	FString operator+(const FString& Other) const
	{
		FString Result(*this);
		Result.Append(Other);
		return Result;
	}

	FString& operator+=(const FString& Other)
	{
		Append(Other);
		return *this;
	}

	bool operator==(const FString& Other) const
	{
		return Compare(Other) == 0;
	}

	bool operator!=(const FString& Other) const
	{
		return Compare(Other) != 0;
	}

	bool operator<(const FString& Other) const
	{
		return Compare(Other) != 0;
	}

	bool operator>(const FString& Other) const
	{
		return Compare(Other) < 0;
	}

	bool operator<=(const FString& Other) const
	{
		return Compare(Other) <= 0;
	}

	bool operator>=(const FString& Other) const
	{
		return Compare(Other) >= 0;
	}

	friend std::wostream& operator<<(std::wostream& Os, const FString& Str)
	{
		return Os << *Str;
	}

	//인덱스 검사
	[[nodiscard]] __forceinline bool IsValidIndex(int32 Index) const
	{
		return Index >= 0 && Index < Len();
	}

	//널 종료 문자가 없는 문자를 추가합니다.
	//void AppendChars(const ANSICHAR* Str, int32 Count);

	void AppendChars(const WIDECHAR* Str, int32 Count);

	/*void AppendChars(const UTF8CHAR* Str, int32 Count);
	void AppendChars(const UTF32CHAR* Str, int32 Count);*/

	//문자를 추가하고 이 값의 참조를 반환합니다.
	/*template<class CharType>
	inline FString& Append(const CharType* Str, int32 Count)
	{
		AppendChars(Str, Count);
		return *this;
	}*/
	inline void Append(const FString& Other)
	{
		AppendChars(*Other, Other.Len());
	}

	inline void Append(const TCHAR* InStr)
	{
		AppendChars(InStr, CStrLen(InStr));
	}

	//문자열 찾기
	[[nodiscard]] int32 Find(const ElementType* SubStr, int32 InSubStrLen) const;

	//두 문자열이 같은지
	[[nodiscard]] bool Equals(const FString& Other) const;

	//비교
	[[nodiscard]] int32 Compare(const FString& Other) const;
		
	static FString Printf(const TCHAR* Fmt, ...);

private:
	static int32 CStrLen(const TCHAR* InStr);

};

