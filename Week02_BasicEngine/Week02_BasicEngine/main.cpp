#include <iostream>
#include "CoreType.h"
#include "TArray.h"

#include "CoreType.h"
#include "FString.h"

int main()
{

	int32 a = 3;
	int32 b = 5;

	std::cout << a + b << std::endl;

	//FString(L"Test");

	FString F1(L"Test1 ");
	F1 += L"Test2";

	std::wcout << F1 << std::endl;

	return 0;
}