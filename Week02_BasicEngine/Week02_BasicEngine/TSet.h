#pragma once
#include <functional>

#include "CoreType.h"
#include "TArray.h"

#define BUCKET_COUNT 128

template <typename T>
class TSet
{
private:
	int32 bucketCount;
	TArray<T> bucket;
	TArray<bool> isBucket;
	int32 elementCount;
	std::hash<T> hashFunction;

	int32 Hash(const T& Item) const // Item을 int32 헤시값으로 변환
	{
		return static_cast<int32>(hashFunction(Item) % bucketCount);
	}

public:
	TSet()
		: bucketCount(BUCKET_COUNT), elementCount(0)
	{
		for (int32 i = 0; i < bucketCount; i++)
		{
			bucket.Add(T{});
			isBucket.Add(false);
		}
	}

	int32 Num() const 
	{
		return elementCount;
	}

	void Add(T Item) // Item을 set에 추가
	{
		int32 hashIndex = Hash(Item);
		if (isBucket[hashIndex])
		{
			// chaining 필요
			elementCount--;
		}
		bucket[hashIndex] = Item;
		isBucket[hashIndex] = true;
		elementCount++;
	}

	bool Contains(T Item) const // Item이 set에 있는지 반환
	{
		int32 hashIndex = Hash(Item);
		if (isBucket[hashIndex] && bucket[hashIndex] == Item)
		{
			return true;
		}
		return false;
	}

	T* Find(T Item) // Item을 찾아 포인터를 반환. 없으면 nullptr 반환
	{
		int32 hashIndex = Hash(Item);
		if (isBucket[hashIndex] && bucket[hashIndex] == Item)
		{
			return &bucket[hashIndex];
		}
		return nullptr;
	}

	bool Remove(T Item) // Item을 찾아 제거. 성공하면 true 반환 
	{
		if (Contains(Item))
		{
			int32 hashIndex = Hash(Item);
			bucket[hashIndex] = T{};
			isBucket[hashIndex] = false;
			elementCount--;
			return true;
		}
		return false;
	}
};