#include <iostream>
#include "TArray.h"

int main()
{
	TArray<int> test;
	test.Add(34);
	test.Add(130);
	std::cout << test[1] << std::endl;
	return 0;
}