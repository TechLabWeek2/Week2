#include <iostream>
#include "TArray.h"

int main()
{
	TArray<int> test;
	test.Add(34);
	test.Add(130);
	std::cout << test.Pop() << std::endl;

	return 0;
}