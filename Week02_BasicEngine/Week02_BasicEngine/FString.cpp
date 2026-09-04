#include "FString.h"

//void FString::AppendChars(const ANSICHAR* Str, int32 Count)
//{
//}

FString::FString(const ElementType* InStr)
{
	int32 InLen = CStrLen(InStr);
	if (InLen > 0)
	{
		AppendChars(InStr, InLen);
	}
}

void FString::AppendChars(const WIDECHAR* Str, int32 Count)
{
	if (Count <= 0) return;

	int32 OldEnd = static_cast<int32>(Data.size());
	//size - 1 자리는 널 문자열 자리다. 거기서부터 쓰기 시작.
	int32 WritePos = OldEnd ? OldEnd - 1 : 0;

	//새로운 널 자리 넣어야 하므로 + 1
	Data.resize(WritePos + Count + 1);

	//Str을 붙여 넣는다.
	std::wmemcpy(Data.data() + WritePos, Str, Count);
	Data[WritePos + Count] = '\0';
}

int32 FString::Find(const ElementType* SubStr, int32 InSubStrLen) const
{
	assert(InSubStrLen >= 0, "Invalid SubStrLen : %d", InSubStrLen);

	int32 SubLen = wcslen(SubStr);
	int32 MyLen = Len();
	if (SubLen == 0 || SubLen > MyLen)
	{
		return -1;
	}

	const TCHAR* Base = Data.data();
	for (int32 i = 0; i <= MyLen - SubLen; i++)
	{
		if (std::memcmp(Base + i, SubStr, static_cast<size_t>(SubLen)) == 0)
		{
			return i;
		}
	}
	return -1;
}

bool FString::Equals(const FString& Other) const
{
	int32 Num = Data.size();
	int32 OtherNum = Other.Data.size();

	if (Num != OtherNum)
	{
		// FString() == FString("") 의 경우 제어.
		return Num + OtherNum == 1;
	}
	else if (Num > 1)
	{

	}

	return true;
}

int32 FString::Compare(const FString& Other) const
{
	int32 MyLen = Len();
	int32 OtherLen = Other.Len();
	//더 작은거
	int32 MinLen = MyLen < OtherLen ? MyLen : OtherLen;

	if (MinLen > 0)
	{
		// 공통 구간까지 원소 단위로 비교 (바이트 비교인 memcmp와 달리
		// wmemcmp는 TCHAR 값 자체를 수치로 비교하므로 대소 순서가 정확함)
		int32 Result = std::wmemcmp(Data.data(), Other.Data.data(), static_cast<size_t>(MinLen));
		if (Result != 0)
		{
			return Result;
		}
	}

	// 공통 구간까지 완전히 같았다면, 더 짧은 쪽이 "작은" 문자열
	// 예시: "Hi" < "Hi!" (길이 차이로 결정)
	return MyLen - OtherLen;

}

int32 FString::CStrLen(const TCHAR* InStr)
{
	if (InStr == nullptr)
	{
		return 0;
	}
	// TCHAR가 wchar_t이므로 wcslen을 씀
	return static_cast<int32>(std::wcslen(InStr));
}
