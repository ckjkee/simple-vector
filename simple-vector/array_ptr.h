#pragma once

template<typename Type>
class ArrayPtr
{
public:
	ArrayPtr() = default;

	explicit ArrayPtr(size_t size) { if (!size) data_ = nullptr; else data_ = new Type[size]; }

	ArrayPtr(const Type* ptr) noexcept : data_(ptr) {}

	ArrayPtr(const ArrayPtr& other) = delete;

	ArrayPtr& operator=(const ArrayPtr& other) = delete;

	void swap(ArrayPtr& other) noexcept
	{
		std::swap(data_, other.data_);
	}

	_NODISCARD Type* Release() noexcept
	{
		auto tmp = data_;
		data_ = nullptr;
		return tmp;
	}

	Type& operator[](size_t index) noexcept
	{
		return data_[index];
	}

	const Type& operator[](size_t index) const noexcept
	{
		return data_[index];
	}

	explicit operator bool() const
	{
		return data_ ? true : false;
	}

	Type* Get() const noexcept
	{
		return data_;
	}

	~ArrayPtr()
	{
		delete[] data_;
	}

private:
	Type* data_ = nullptr;
};
