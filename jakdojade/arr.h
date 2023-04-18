#pragma once

#include "vector.h"

template<typename T>
class arr
{
private:
    Vector<T> buffer_;
    int rows_;
    int cols_;

public:
    // Constructor
    arr(int rows, int cols) : buffer_(rows * cols), rows_(rows), cols_(cols) {}

    int get_rows() const { return rows_; }
    int get_cols() const { return cols_; }

    // Overload operator[] to access elements in the 1D buffer as if they were arranged in a 2D grid
    class Proxy {
    public:
        Proxy(T* data, int cols) : data_(data), cols_(cols) {}

        T& operator[](int col) {
            return data_[col];
        }

    private:
        T* data_;
        int cols_;
    };

    Proxy operator[](int row) {
        return Proxy(&buffer_[row * cols_], cols_);
    }
};
