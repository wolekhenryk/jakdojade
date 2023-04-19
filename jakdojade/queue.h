#pragma once

#include "vector.h"

template <typename T>
class queue {
public:
    void push(const T &value) { data_.push_back(value); }

    void pop() { data_.erase(data_.begin()); }

    T &front() { return data_.front(); }

    T &back() { return data_.back(); }

    bool empty() const { return data_.empty(); }

    [[nodiscard]] size_t size() const { return data_.size(); }

private:
    Vector<T> data_;
};