#pragma once

#include "vector.h"

template<typename T>
struct greater_fn
{
    bool operator()(const T& a, const T& b) const
    {
        return a > b;
    }
};

template <typename T, typename Compare = greater_fn<T>>
class heap
{
private:
    Vector<T> heap_;
    Compare const& compare_;

    std::size_t parent(std::size_t i) { return (i - 1) / 2; }

    std::size_t left(std::size_t i) { return 2 * i + 1; }

    std::size_t right(std::size_t i) { return 2 * i + 2; }

    void swap(std::size_t i, std::size_t j)
    {
        T temp = std::move(heap_[i]);
        heap_[i] = std::move(heap_[j]);
        heap_[j] = std::move(temp);
    }

    void heapify_up(std::size_t i) {
        while (i != 0 && compare_(heap_[i], heap_[parent(i)])) {
            if (compare_(heap_[i], heap_[parent(i)])) {
                break;
            }
            swap(parent(i), i);
            i = parent(i);
        }
    }

    void heapify_down(const std::size_t i) {
        std::size_t l = left(i);
        std::size_t r = right(i);
        std::size_t smallest = i;
        if (l < heap_.size() && compare_(heap_[l], heap_[smallest]))
            smallest = l;
        if (r < heap_.size() && compare_(heap_[r], heap_[smallest]))
            smallest = r;
        if (smallest != i) {
            if (compare_(heap_[smallest], heap_[i])) {
                return;
            }
            swap(i, smallest);
            heapify_down(smallest);
        }
    }

public:
    heap() = default;

    explicit heap(std::size_t reserve_size)
        : heap_()
        , compare_(Compare())
    {
        heap_.reserve(reserve_size);
    }

    explicit heap(const Compare& compare, std::size_t reserve_size)
        : heap_()
        , compare_(compare)
    {
        heap_.reserve(reserve_size);
    }

    void push(const T& value)
    {
        heap_.push_back(value);
        const std::size_t index = heap_.size() - 1;
        heapify_up(index);
    }

    void pop()
    {
        if (heap_.size() == 0) return;
        heap_[0] = heap_.back();
        heap_.pop_back();
        heapify_down(0);
    }

    T top() const
    {
        if (heap_.size() == 0) return T();
        return heap_[0];
    }

    [[nodiscard]] bool empty() const { return heap_.size() == 0; }

    [[nodiscard]] int size() const { return heap_.size(); }
};