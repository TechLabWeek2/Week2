#pragma once
#include "CoreType.h"

template <typename T>
class TArray
{
private:
	T* array;
	int32 arrayNum; // array의 item 개수
	int32 arrayMax; // array의 최대 크기

	void AllocateMemory() // array의 공간이 부족한 경우 메모리를 재할당
	{
		if (arrayNum == 0)
		{
			array = new T[arrayMax];
		}
		else if (arrayNum >= arrayMax - 1)
		{
			T* newArray = new T[arrayMax * 2];
			for (int32 i = 0; i < arrayNum; i++)
			{
				newArray[i] = array[i];
			}
			delete[] array;
			array = newArray;
			arrayMax *= 2;
		}
	}
	void IndexCheck(int32 index) const // input으로 들어온 index가 범위안에 있는지 확인
	{
		if (index < 0 || index > arrayMax)
		{
			throw("Index out of range");
		}
	}

public:
	TArray()
		: array(nullptr), arrayNum(0), arrayMax(10)
	{}

	void Add(T item)
	{
		AllocateMemory();

		array[arrayNum++] = item;
	}

	void Insert(T item, int32 index)
	{
		IndexCheck(index);
		AllocateMemory();

		for (int32 i = arrayNum; i > index; i--)
		{
			array[i + 1] = array[i];
		}
		array[index] = item;
		arrayNum++;
	}

	T Pop()
	{
		T poppedValue = array[arrayNum];

		arrayNum--;
		return poppedValue;
	}
};