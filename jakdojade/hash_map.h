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
constexpr size_t k_num_buckets = 107;

template <typename T>
class hasher
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
	class iterator
	{
	public:
		iterator(hash_map<K, V>& map, size_t bucket_idx, typename Vector<k_v_pair<K, V>>::iterator pair_iter)
			: map_(map), bucket_idx_(bucket_idx), pair_iter_(pair_iter) {}

		bool operator==(const iterator& other) const
		{
			return (bucket_idx_ == other.bucket_idx_ && pair_iter_ == other.pair_iter_);
		}

		bool operator!=(const iterator& other) const
		{
			return !(*this == other);
		}

		iterator& operator++()
		{
			++pair_iter_;
			if (pair_iter_ == map_.buckets_[bucket_idx_].end())
			{
				do
				{
					++bucket_idx_;
				} while (bucket_idx_ < map_.buckets_.size() && map_.buckets_[bucket_idx_].empty());
				if (bucket_idx_ < map_.buckets_.size())
				{
					pair_iter_ = map_.buckets_[bucket_idx_].begin();
				}
			}
			return *this;
		}

		k_v_pair<K, V>& operator*() const
		{
			return *pair_iter_;
		}

		k_v_pair<K, V>* operator->() const
		{
			return &(*pair_iter_);
		}

	private:
		hash_map<K, V>& map_;
		size_t bucket_idx_;
		typename Vector<k_v_pair<K, V>>::iterator pair_iter_;
	};

	hash_map() : buckets_(k_num_buckets), size(0) {}

	void insert(const K& key, const V& value)
	{
		const auto hash_value = hasher<K>::make(key) % k_num_buckets;
		for (auto& pair : buckets_[hash_value])
		{
			if (pair.key == key)
			{
				pair.value = value;
				return;
			}
		}
		buckets_[hash_value].push_back({ key, value });
		size++;
	}

	V& operator[](const K& key)
	{
		const auto hash_value = hasher<K>::make(key) % k_num_buckets;
		for (auto& pair : buckets_[hash_value]) {
			if (pair.key == key) {
				return pair.value;
			}
		}
		buckets_[hash_value].push_back({ key, V() });
		size++;

		return buckets_[hash_value].back().value;
	}

	V& at(const K& key)
	{
		const auto hash_value = hasher<K>::make(key) % k_num_buckets;
		for (auto& pair : buckets_[hash_value])
		{
			if (pair.key == key)
			{
				return pair.value;
			}
		}

		throw std::out_of_range("key not found in hash_map");
	}

	V& at(const K& key) const
	{
		const auto hash_value = hasher<K>::make(key) % k_num_buckets;
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
		const auto hash_value = hasher<K>::make(key) % k_num_buckets;
		for (const auto& pair : buckets_[hash_value]) {
			if (pair.key == key) {
				return true;
			}
		}
		return false;
	}

	void erase(const K& key)
	{
		const auto hash_value = hasher<K>::make(key) % k_num_buckets;
		auto& bucket = buckets_[hash_value];
		for (auto iter = bucket.begin(); iter != bucket.end(); ++iter) {
			if (iter->key == key) {
				bucket.erase(iter);
				size--;
				return;
			}
		}
	}

	size_t get_size() const {
		return this->size;
	}

	Vector<Vector<k_v_pair<K, V>>> get_buckets() const {
		return buckets_;
	}

	typename Vector<k_v_pair<K, V>>::iterator find(const K& key)
	{
		const auto hash_value = hasher<K>::make(key) % k_num_buckets;
		auto& bucket = buckets_[hash_value];

		for (auto it = bucket.begin(); it != bucket.end(); ++it)
		{
			if (it->key == key) return it;
		}

		return bucket.end();
	}

	using it = typename Vector<k_v_pair<K, V>>::iterator;

	it begin() {
		for (auto& bucket : buckets_) {
			if (!bucket.empty()) return bucket.begin();
		}
		return end();
	}

	it end() {
		return buckets_.back().end();
	}
	
private:
	Vector<Vector<k_v_pair<K, V>>> buckets_;
	size_t size;
};


  