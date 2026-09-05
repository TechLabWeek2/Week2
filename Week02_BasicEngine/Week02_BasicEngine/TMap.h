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
	{}

	V& operator[] (const K& key)
	{
		return map[key];
	}

	int32 Num() const
	{
		return static_cast<int32>(map.size());
	}

	void Emplace(const K& key, const V& value)
	{
		map.emplace(key, value);
	}

	void Add(const K& key, const V& value)
	{
		map[key] = value;
	}

	V* Find(const K& key)
	{
		auto iter = map.find(key);
		if (iter != map.end())
		{
			return &iter->second;
		}
		return nullptr;
	}

	const V* Find(const K& key) const
	{
		auto iter = map.find(key);
		if (iter != map.end())
		{
			return &iter->second;
		}
		return nullptr;
	}

	bool Contains(const K& key) const
	{
		return map.count(key) > 0;
	}

	bool Remove(const K& key)
	{
		return map.erase(key) > 0;
	}

	void Empty()
	{
		map.clear();
	}
};