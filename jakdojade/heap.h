#pragma once

#include "vector.h"

template <typename T>
class heap
{
private:
	Vector<T> heap_;

	int parent(const int i) { return (i - 1) / 2; }

	int left(const int i) { return 2 * i + 1; }

	int right(const int i) { return 2 * i + 2; }

	void swap(int i, int j)
	{
		T temp = heap_[i];
		heap_[i] = heap_[j];
		heap_[j] = heap_[i];
	}

	void heapify_up(int i) {
		while (i != 0 && heap[parent(i)] > heap[i]) {
			swap(parent(i), i);
			i = parent(i);
		}
	}

	void heapify_down(const int i) {
		int l = left(i);
		int r = right(i);
		int smallest = i;
		if (l < heap_.size() && heap_[l] < heap_[smallest])
			smallest = l;
		if (r < heap_.size() && heap_[r] < heap_[smallest])
			smallest = r;
		if (smallest != i) {
			swap(i, smallest);
			heapify_down(smallest);
		}
	}

public:
	heap() {}

	void push(const T& value)
	{
		heap_.push_back(value);
		const int index = heap_.size() - 1;
		heapify_up(index);
	}

	void pop()
	{
		if (heap_.size() == 0) return;
		heap_[0] = heap_.back();
		heap_.pop_back();
		heapify_down(0);
	}

	T& top() const
	{
		if (heap_.size() == 0) return T();
		return heap[0];
	}

	[[nodiscard]] bool empty() const { return heap_.size() == 0; }

	[[nodiscard]] int size() const { return heap_.size(); }
};