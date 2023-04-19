#pragma once

#include "vector.h"

template <typename T>
class arr {
private:
    Vector<T> buffer_;
    int rows_;
    int cols_;

public:
    arr(const int rows, const int cols) : buffer_(rows * cols), rows_(rows), cols_(cols) {}

    [[nodiscard]] int get_rows() const { return rows_; }
    [[nodiscard]] int get_cols() const { return cols_; }

    class proxy {
    public:
        proxy(T *data, const int cols) : data_(data), cols_(cols) {}

        T &operator[](int col) { return data_[col]; }

    private:
        T *data_;
        int cols_;
    };

    proxy operator[](const int row) { return proxy(&buffer_[row * cols_], cols_); }
};
