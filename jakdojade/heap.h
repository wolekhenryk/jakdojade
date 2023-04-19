#pragma once

#include "vector.h"

using std::size_t;

template <typename T>
struct greater_fn {
    bool operator()(const T &a, const T &b) const { return a > b; }
};

template <typename T, typename Compare>
class heap {
private:
    Vector<T> heap_;
    Compare const &compare_;

    static size_t parent(const size_t i) { return (i - 1) / 2; }

    static size_t left(const size_t i) { return 2 * i + 1; }

    static size_t right(const size_t i) { return 2 * i + 2; }

    void swap(std::size_t i, std::size_t j) {
        T temp = std::move(heap_[i]);
        heap_[i] = std::move(heap_[j]);
        heap_[j] = std::move(temp);
    }

    void heapify_up(size_t i) {
        while (i != 0 && compare_(heap_[i], heap_[parent(i)])) {
            swap(parent(i), i);
            i = parent(i);
        }
    }

    void heapify_down(size_t i) {
        size_t smallest = i;
        while (true) {
            const size_t l = left(smallest);
            const size_t r = right(smallest);
            if (l < heap_.size() && compare_(heap_[l], heap_[smallest]))
                smallest = l;
            if (r < heap_.size() && compare_(heap_[r], heap_[smallest]))
                smallest = r;
            if (smallest != i) {
                if (compare_(heap_[smallest], heap_[i])) { break; }
                swap(i, smallest);
                i = smallest;
            } else {
                break;
            }
        }
    }

public:
    heap() : compare_(Compare()) {}

    explicit heap(size_t reserve_size) : heap_(), compare_(Compare()) {
        heap_.reserve(reserve_size + 128);
    }

    explicit heap(const Compare &compare, size_t reserve_size)
        : heap_(), compare_(compare) {
        heap_.reserve(reserve_size);
    }

    void push(const T &value) {
        heap_.push_back(value);
        const size_t index = heap_.size() - 1;
        heapify_up(index);
    }

    void pop() {
        if (heap_.empty()) return;
        heap_[0] = heap_.back();
        heap_.pop_back();
        heapify_down(0);
    }

    [[nodiscard]] T top() const {
        if (heap_.empty()) return T();
        return heap_[0];
    }

    [[nodiscard]] bool empty() const { return heap_.empty(); }

    [[nodiscard]] int size() const { return heap_.size(); }
};