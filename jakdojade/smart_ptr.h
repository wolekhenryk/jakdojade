#pragma once

#include <utility>
#include <memory>

template <class T>
class smart_ptr
{
public:
	smart_ptr() noexcept : smart_ptr(nullptr) {}
	explicit smart_ptr(T* ptr) noexcept : ptr_(ptr){}

	smart_ptr(const smart_ptr&) = delete;
	smart_ptr& operator=(const smart_ptr&) = delete;

	smart_ptr(smart_ptr&& other) noexcept : ptr_(other.release()) {}

	smart_ptr& operator=(smart_ptr&& other) noexcept
	{
		if (this != &other) reset(other.release());
		return *this;
	}

	T* release() noexcept { return std::exchange(ptr_, nullptr); }

	void reset(T* ptr = nullptr) noexcept
	{
		T* old = std::exchange(ptr_, ptr);
		if (old) delete old;
	}

	T& operator[](std::size_t index) const noexcept {
		return ptr_[index];
	}

	T& operator[](std::size_t index) noexcept {
		return ptr_[index];
	}

	T& operator*() const noexcept { return *ptr_; }

	T* operator->() const noexcept { return ptr_; }

	T* get() const noexcept { return ptr_; }

	explicit operator bool() const noexcept { return static_cast<bool>(ptr_); }

	template<typename... Args>
	static smart_ptr<T> make_unique(Args&&... args)
	{
		return smart_ptr<T>(new T(std::forward<Args>(args)...));
	}

	~smart_ptr() noexcept { delete ptr_; }

private:
	T* ptr_;
};
