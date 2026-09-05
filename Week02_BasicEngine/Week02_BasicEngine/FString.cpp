#include "FString.h"
#include <cstdarg>

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

	int32 OldEnd = static_cast<int32>(Data.Num());
	//Num - 1 자리는 널 문자열 자리다. 거기서부터 쓰기 시작.
	int32 WritePos = OldEnd ? OldEnd - 1 : 0;

	//새로운 널 자리 넣어야 하므로 + 1
	Data.Resize(WritePos + Count + 1);

	//Str을 붙여 넣는다.
	std::wmemcpy(Data.Data() + WritePos, Str, Count);
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

	const TCHAR* Base = Data.Data();
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
	int32 Num = Data.Num();
	int32 OtherNum = Other.Data.Num();

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
		int32 Result = std::wmemcmp(Data.Data(), Other.Data.Data(), static_cast<size_t>(MinLen));
		if (Result != 0)
		{
			return Result;
		}
	}

	// 공통 구간까지 완전히 같았다면, 더 짧은 쪽이 "작은" 문자열
	// 예시: "Hi" < "Hi!" (길이 차이로 결정)
	return MyLen - OtherLen;

}

FString FString::Printf(const TCHAR* Fmt, ...)
{
	va_list Args;
	va_start(Args, Fmt);

	// vsnprintf(narrow)와 달리 vswprintf(wide)는 "버퍼가 부족할 때 필요한 크기를
	// 알려주는" 동작이 표준/플랫폼마다 일관되지 않고, 실패 시 그냥 음수만 반환할
	// 수 있다. 그래서 "일단 시도 -> 부족하면 버퍼를 2배로 키워서 재시도" 방식으로
	// 안전하게 처리한다.
	int32 BufferSize = 256;

	while (true)
	{
		TCHAR* Buffer = new TCHAR[BufferSize];

		va_list ArgsCopy;
		va_copy(ArgsCopy, Args);
		int32 Written = static_cast<int32>(std::vswprintf(Buffer, static_cast<size_t>(BufferSize), Fmt, ArgsCopy));
		va_end(ArgsCopy);

		if (Written >= 0 && Written < BufferSize)
		{
			// 성공: Written은 널 종단자를 제외한 실제 글자 수
			FString Result(Buffer);
			delete[] Buffer;
			va_end(Args);
			return Result;
		}

		// 버퍼가 부족했거나 실패(음수) -> 버퍼를 2배로 늘려서 재시도
		delete[] Buffer;
		BufferSize *= 2;

		if (BufferSize > (1 << 20)) // 100만 글자 넘어가면 무한루프 방지용으로 포기
		{
			va_end(Args);
			return FString(L"[FString::Printf Error: format too long]");
		}
	}
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
