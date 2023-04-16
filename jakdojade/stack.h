#pragma once

#include "vector.h"

template<typename T>

class stack
{
public:
	void push(const T& value)
	{
		data_.push_back(value);
	}

	T& pop()
	{
		T& value = data_.back();
		data_.pop_back();
		return value;
	}

	T& top()
	{
		return data_.back();
	}

	[[nodiscard]] bool empty() const
	{
		return data_.empty();
	}

	[[nodiscard]] int size() const
	{
		return data_.size();
	}

private:
	Vector<T> data_;
};