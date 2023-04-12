#pragma once

#define _CRT_SECURE_NO_WARNINGS

template<typename T>
class Vector
{
    T* data_;
    int size_;
    int capacity_;
public:
    Vector() : size_{ 0 }, capacity_{ 0 }, data_{ nullptr } {}

    explicit Vector(const int size) : size_{ size }, capacity_{ size }, data_{ new T[size] } {}

    Vector(size_t size, const T& value) : Vector(size) {
        for (size_t i = 0; i < size_; i++) {
            data_[i] = value;
        }
    }
    Vector(std::initializer_list<T> values) : Vector(values.size()) {
        std::copy(values.begin(), values.end(), data_);
    }
    Vector(const Vector<T>& other) : Vector(other.size_) {
        std::copy(other.data_, other.data_ + other.size_, data_);
    }
    Vector(Vector<T>&& other) noexcept : size_{ other.size_ }, capacity_{ other.capacity_ }, data_{ other.data_ } {
        other.size_ = 0;
        other.capacity_ = 0;
        other.data_ = nullptr;
    }
    ~Vector() {
        clear();
    }

    Vector<T>& operator=(const Vector<T>& other) {
        if (this != &other) {
            clear();
            reserve(other.size_);
            std::copy(other.data_, other.data_ + other.size_, data_);
            size_ = other.size_;
        }
        return *this;
    }

    Vector<T>& operator=(Vector<T>&& other) noexcept {
        if (this != &other) {
            clear();
            size_ = other.size_;
            capacity_ = other.capacity_;
            data_ = other.data_;
            other.size_ = 0;
            other.capacity_ = 0;
            other.data_ = nullptr;
        }
        return *this;
    }

    void clear() noexcept {
        size_ = 0;
        capacity_ = 0;
        delete[] data_;
        data_ = nullptr;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            T* new_data = new T[new_capacity];
            if (data_) {
                std::copy(data_, data_ + size_, new_data);
                delete[] data_;
            }
            data_ = new_data;
            capacity_ = new_capacity;
        }
    }

    void resize(size_t new_size, const T& value = T()) {
        if (new_size < size_) {
            for (size_t i = new_size; i < size_; i++) {
                data_[i].~T();
            }
            size_ = new_size;
        }
        else if (new_size > size_) {
            reserve(new_size);
            for (size_t i = size_; i < new_size; i++) {
                new (&data_[i]) T(value);
            }
            size_ = new_size;
        }
    }

    void push_back(const T& value) {
        if (size_ == capacity_) {
            reserve(capacity_ == 0 ? 1 : 2 * capacity_);
        }
        new (&data_[size_]) T(value);
        size_++;
    }

    void pop_back() {
        if (size_ > 0) {
            data_[size_ - 1].~T();
            size_--;
        }
    }

    T& operator[](size_t index) {
        return data_[index];
    }

    const T& operator[](size_t index) const {
        return data_[index];
    }

    size_t size() const noexcept {
        return size_;
    }

    size_t capacity() const noexcept {
        return capacity_;
    }

    T& front() const
    {
        return data_[0];
    }

    T& back() const
    {
        return data_[size_ - 1];
    }

    void erase(const int index)
    {
        if (index < 0 || index >= size_) return;

        for (int i = index + 1; i < size_; i++) data_[i - 1] = data_[i];

        size_--;
    }

    class reverse_iterator
    {
        T* ptr_;

    public:
        explicit reverse_iterator(T* ptr) : ptr_(ptr) {}

        T& operator*() const { return *ptr_; }
        T* operator->() const { return  ptr_; }

        bool operator==(const reverse_iterator& other) const { return ptr_ == other.ptr_; }
        bool operator!=(const reverse_iterator& other) const { return ptr_ != other.ptr_; }

        reverse_iterator& operator++()
        {
            --ptr_;
            return *this;
        }
    };

    reverse_iterator rbegin() { return reverse_iterator(data_ + size_ - 1); }
    reverse_iterator rend() { return reverse_iterator(data_ - 1); }

    class iterator {
        T* current_;

    public:
        explicit iterator(T* ptr) : current_(ptr) {}
        T& operator*() const { return *current_; }
        T* operator->() const { return current_; }
        iterator& operator++() { ++current_; return *this; }
        iterator operator++(int) { iterator temp(current_); ++current_; return temp; }
        iterator& operator--() { --current_; return *this; }
        iterator operator--(int) { iterator temp(current_); --current_; return temp; }
        bool operator==(const iterator& other) const { return current_ == other.current_; }
        bool operator!=(const iterator& other) const { return !(*this == other); }
    };

    iterator begin() const { return iterator(data_); }
    iterator end() const { return iterator(data_ + size_); }

    void erase(iterator it)
    {
        if (it == end()) return;

        iterator next_it = it;
        ++next_it;

        while (next_it != end())
        {
            *it = *next_it;
            ++it;
            ++next_it;
        }

        --size_;
    }
};