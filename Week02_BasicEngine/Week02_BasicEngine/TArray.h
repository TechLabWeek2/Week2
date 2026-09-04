#pragma once
#include "CoreType.h"

#define DATA_MAX_INIT 10

template <typename T>
class TArray
{
private:
	T* data;
	int32 dataNum; // data의 item 개수
	int32 dataMax; // data의 최대 크기

	void AllocateMemory() // data의 공간이 부족한 경우 메모리를 재할당
	{
		if (data == nullptr)
		{
			data = new T[dataMax];
		}
		else if (dataNum >= dataMax)
		{
			T* newdata = new T[dataMax * 2];
			for (int32 i = 0; i < dataNum; i++)
			{
				newdata[i] = data[i];
			}
			delete[] data;
			data = newdata;
			dataMax *= 2;
		}
	}
	void IndexCheck(int32 Index) const // input으로 들어온 index가 범위안에 있는지 확인
	{
		if (Index < 0 || Index >= dataNum)
		{
			throw std::out_of_range("Index of out range");
		}
	
	}
	bool IsEmpty() const // data가 비어있는지 확인
	{
		return dataNum <= 0;
	}

public:
	TArray()
		: data(nullptr), dataNum(0), dataMax(DATA_MAX_INIT)
	{}

	TArray(const TArray& otherArray) // 복사 생성자
		: data(nullptr), dataNum(0), dataMax(DATA_MAX_INIT)
	{
		if (otherArray.data)
		{
			this->data = new T[otherArray.Max()];
			for (int32 i = 0; i < otherArray.Num(); i++)
			{
				this->data[i] = otherArray.data[i];
			}
			this->dataNum = otherArray.Num();
			this->dataMax = otherArray.Max();
		}
	}

	TArray& operator= (const TArray& otherArray) // = : 깊은 복사 
	{
		if (this == &otherArray)
		{
			return *this;
		}

		delete[] this->data;
		if (otherArray.data)
		{
			this->data = new T[otherArray.Max()];
			for (int32 i = 0; i < otherArray.Num(); i++)
			{
				this->data[i] = otherArray.data[i];
			}
			this->dataNum = otherArray.Num();
			this->dataMax = otherArray.Max();
		}
		else
		{
			this->data = nullptr;
			this->dataNum = 0;
			this->dataMax = DATA_MAX_INIT;
		}
		return *this;
	}

	~TArray()
	{
		delete[] data;
	}

	T& operator[] (int32 Index) const
	{
		IndexCheck(Index);
		return data[Index];
	}

	T& operator[] (int32 Index)
	{
		IndexCheck(Index);
		return data[Index];
	}

	int32 Num() const // data의 Item 개수를 반환
	{
		return dataNum;
	}

	int32 Max() const // data의 최대 크기를 반환
	{
		return dataMax;
	}

	void Add(T Item) // Item을 data에 추가
	{
		AllocateMemory();

		data[dataNum++] = Item;
	}

	void Insert(T Item, int32 Index) // Item을 특정 Index에 추가
	{
		if (Index < 0 || Index > dataNum)
		{
			throw std::out_of_range("Index of out range");
		}
		AllocateMemory();

		for (int32 i = dataNum - 1; i > Index; i--)
		{
			data[i + 1] = data[i];
		}
		data[Index] = Item;
		dataNum++;
	}

	T Pop() // 마지막 Item을 Pop
	{
		if (IsEmpty())
		{
			throw std::out_of_range("Pop() on empty data");
		}

		T poppedValue = data[dataNum - 1];
		dataNum--;

		return poppedValue;
	}

	void RemoveAt(int32 Index) // 특정 Index의 Item을 제거
	{
		if (IsEmpty())
		{
			throw std::out_of_range("RemoveAt() on empty data");
		}
		IndexCheck(Index);

		for (int32 i = Index; i < dataNum - 1; i++)
		{
			data[i] = data[i + 1];
		}
		dataNum--;
	}

	void RemoveAtSwap(int32 Index) // 특정 Index의 Item을 tail 원소와 swap으로 제거 O(1)
	{
		if (IsEmpty())
		{
			throw std::out_of_range("RemoveAtSwap() on empty data");
		}
		IndexCheck(Index);
		data[Index] = data[dataNum - 1];
		dataNum--;
	}

	int32 Find(const T Item) const // 첫번째로 찾은 Item의 Index를 반환. 없으면 -1 반환
	{
		for (int32 i = 0; i < dataNum; i++)
		{
			if (data[i] == Item)
			{
				return i;
			}
		}
		return -1;
	}

	void Remove(const T Item) // 첫번째로 찾은 Item을 제거
	{
		int32 foundIndex = Find(Item);

		if (foundIndex == -1)
		{
			throw std::out_of_range("Remove() couldn't find Item");
		}

		RemoveAt(foundIndex);
	}
};