#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include <typeinfo>
#include <type_traits>

#include "vector.h"
#include "str.h"

using std::size_t;

constexpr int first_prime_number = 96134671;
constexpr int second_prime_number = 80749733;

constexpr size_t starting_hash = 5381;
constexpr size_t k_num_buckets = 3000007;

template <typename T>
class hash
{
public:
	static size_t make(const T& value)
	{
		if constexpr (std::is_convertible_v<T, size_t>)
		{
			return static_cast<size_t>(value) % first_prime_number;
		}

		if (typeid(value) == typeid(str))
		{
			const str m_str(value);
			size_t hash = starting_hash;

			for (size_t i = 0; i < m_str.length(); i++)
			{
				hash = ((hash << 5) + 5) + static_cast<size_t>(m_str[i]);
			}

			return hash % second_prime_number;
		}

		return 0;
	}
};

template <typename K, typename V>
struct k_v_pair
{
	K key;
	V value;
};

template <typename K, typename V>
class hash_map
{
public:
	hash_map() : buckets_(k_num_buckets) {}

	void insert(const K& key, const V& value)
	{
		const auto hash_value = hash<K>::make(key) % k_num_buckets;
		for (auto& pair : buckets_[hash_value])
		{
			if (pair.key == key)
			{
				pair.value = value;
				return;
			}
		}
		buckets_[hash_value].push_back({ key, value });
	}

	V& operator[](const K& key)
	{
		const auto hash_value = hash<K>::make(key) % k_num_buckets;
		for (auto& pair : buckets_[hash_value]) {
			if (pair.key == key) {
				return pair.value;
			}
		}
		buckets_[hash_value].push_back({ key, V() });

		return buckets_[hash_value].back().value;
	}

	V& at(const K& key) const 
	{
		const auto hash_value = hash<K>::make(key) % k_num_buckets;
		for (auto& pair : buckets_[hash_value])
		{
			if (pair.key == key)
			{
				return pair.value;
			}
		}
		throw std::out_of_range("key not found in hash_map");
	}

	[[nodiscard]] bool contains(const K& key) const
	{
		const auto hash_value = hash<K>::make(key) % k_num_buckets;
		for (const auto& pair : buckets_[hash_value]) {
			if (pair.key == key) {
				return true;
			}
		}
		return false;
	}

	void erase(const K& key)
	{
		const auto hash_value = hash<K>::make(key) % k_num_buckets;
		auto& bucket = buckets_[hash_value];
		for (auto iter = bucket.begin(); iter != bucket.end(); ++iter) {
			if (iter->key == key) {
				bucket.erase(iter);
				return;
			}
		}
	}

private:
	Vector<Vector<k_v_pair<K, V>>> buckets_;
};
