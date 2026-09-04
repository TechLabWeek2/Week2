#pragma once
#include <unordered_map>

#include "CoreType.h"

template <typename K, typename V>
class TMap
{
private:
	std::unordered_map<K, V> map;

public:
	TMap()
	{
	}

	TMap operator[] (const K& key) const
	{
		return map[key];
	}

	void Add(K key, V value)
	{
		map[key] = value;
	}

	V* Find(K key)
	{
		
	}
};