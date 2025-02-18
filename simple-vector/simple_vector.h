#pragma once

#include "ArrayPtr.h"
#include <cassert>
#include <algorithm>


class ReserveObject
{
public:
	ReserveObject(size_t size) : capacity_(size) {}

	size_t GetCapacity() const noexcept
	{
		return capacity_;
	}

private:
	size_t capacity_;
};

ReserveObject Reserve(const ReserveObject& obj) 
{
	return obj.GetCapacity();
}

template<typename Type>
class SimpleVector
{
public:
	using value_type = Type;
	using iterator = Type*;
	using const_iterator = const Type*;

	SimpleVector() noexcept = default;

	explicit SimpleVector(const ReserveObject& obj)
	{
		Reserve(obj.GetCapacity());
	}

	explicit SimpleVector(size_t size) : size_(size), capacity_(size)
	{
		if (size)
		{
			ArrayPtr<Type> tmp{ size };
			std::fill(tmp.Get(), tmp.Get() + size, Type());
			data_.swap(tmp);
		}
	}

	explicit SimpleVector(size_t new_size, const Type& val) : size_(new_size), capacity_(new_size)
	{
		if (size_)
		{
			ArrayPtr<Type> tmp{ new_size };
			std::fill(tmp.Get(), tmp.Get() + new_size, Type(val));
			data_.swap(tmp);
		}
	}

	explicit SimpleVector(std::initializer_list<Type> list) : size_(list.size()), capacity_(list.size())
	{
		ArrayPtr<Type> tmp{ list.size() };
		std::copy(list.begin(), list.end(), tmp.Get());
		data_.swap(tmp);

	}

	SimpleVector(const SimpleVector& other) : size_(other.size_), capacity_(other.capacity_)
	{
		ArrayPtr<Type> tmp{ other.size_ };
		std::copy(other.cbegin(), other.cend(), tmp.Get());
		data_.swap(tmp);
	}

	SimpleVector& operator=(const SimpleVector& other)
	{
		auto copy(other);
		swap(copy);
		return *this;
	}

	SimpleVector(SimpleVector&& other)
	{
		size_ = std::exchange(other.size_, 0);
		capacity_ = std::exchange(other.capacity_, 0);
		data_.swap(other.data_);
	}

	SimpleVector& operator=(SimpleVector&& other)
	{
		if (data_.Get() != other.data_.Get())
		{
			size_ = std::exchange(other.size_, 0);
			capacity_ = std::exchange(other.capacity_, 0);
			data_.swap(other.data_);
		}
		return *this;
	}

	~SimpleVector() {}

	void swap(SimpleVector& other) noexcept
	{
		std::swap(size_, other.size_);
		std::swap(capacity_, other.capacity_);
		data_.swap(other.data_);
	}

	iterator PushBack(const Type& val)
	{
		return Insert(cend(), val);
	}

	iterator PushBack(Type&& val)
	{
		return Insert(end(), std::move(val));
	}

	void Reserve(size_t new_capacity)
	{
		if (new_capacity <= capacity_) return;
		ArrayPtr<Type> tmp{ new_capacity };
		std::copy(cbegin(), cend(), tmp.Get());
		data_.swap(tmp);
		capacity_ = new_capacity;
	}

	iterator Insert(const_iterator pos, Type&& val)
	{
		iterator new_elem = nullptr;
		if (size_ == capacity_)
		{
			capacity_ = capacity_ ? capacity_ * 2 : 1;
			ArrayPtr<Type> tmp{ capacity_ };
			new_elem = std::move(begin(), iterator(pos), tmp.Get());
			*new_elem = std::move(val);
			std::move(iterator(pos), end(), std::next(new_elem));
			data_.swap(tmp);
		}
		else
		{
			std::move_backward(iterator(pos), end(), end());
			*(iterator(pos)) = std::move(val);
			new_elem = iterator(pos);
		}
		++size_;

		return new_elem;
	}

	iterator Insert(const_iterator pos, const Type& val)
	{
		iterator new_elem = nullptr;
		if (size_ == capacity_)
		{
			capacity_ = capacity_ ? capacity_ * 2 : 1;
			ArrayPtr<Type> tmp{ capacity_ };
			new_elem = std::copy(cbegin(), pos, tmp.Get());
			*new_elem = val;
			std::copy(pos, cend(), std::next(new_elem));
			data_.swap(tmp);
		}
		else
		{
			std::copy_backward(pos, cend(), end());
			*(iterator(pos)) = val;
			new_elem = iterator(pos);
		}
		++size_;

		return new_elem;
	}

	void PopBack() noexcept
	{
		if (!size_) return;
		--size_;
	}

	iterator Erase(iterator pos)
	{
		if (!size_) return iterator{ nullptr };
		std::move(std::next(pos), end(), pos);
		--size_;
		return iterator(pos);
	}

	iterator Erase(const_iterator pos)
	{
		if (!size_) return iterator{ nullptr };
		std::copy(std::next(pos),cend(), iterator(pos));
		--size_;
		return iterator(pos);
	}

	void Resize(size_t new_size)
	{
		if (new_size <= capacity_)
		{
			if (new_size <= size_)
			{
				size_ = new_size;
			}
			else
			{
				std::fill(begin() + size_, begin() + new_size, Type());
				size_ = new_size;
			}
		}
		else if (new_size > capacity_)
		{
			capacity_ = new_size;
			ArrayPtr<Type> new_data{ capacity_ };
			std::copy(begin(), end(), new_data.Get());
			std::fill(new_data.Get() + size_, new_data.Get() + new_size, Type());
			data_.swap(new_data);
			size_ = new_size;
		}
	}

	size_t GetSize() const noexcept
	{
		return size_;
	}

	size_t GetCapacity() const noexcept
	{
		return capacity_;
	}

	bool IsEmpty() const noexcept
	{
		return size_ ? false : true;
	}


	Type& operator[](size_t index) noexcept
	{
		return data_[index];
	}

	const Type& operator[](size_t index) const noexcept
	{
		return data_[index];
	}

	Type& At(size_t index)
	{
		return index >= size_ ? throw std::out_of_range("Out of range") : data_[index];
	}

	const Type& At(size_t index) const
	{
		return index > size_ ? throw std::out_of_range("Out of range") : data_[index];
	}

	void Clear() noexcept
	{
		size_ = 0;
	}

	_NODISCARD iterator begin() noexcept
	{
		return data_.Get();
	}

	_NODISCARD iterator end() noexcept
	{
		return data_.Get() + size_;
	}

	_NODISCARD const_iterator cbegin() const noexcept
	{
		return data_.Get();
	}

	_NODISCARD const_iterator cend() const noexcept
	{
		return data_.Get() + size_;
	}

private:
	ArrayPtr<Type> data_{};
	size_t size_ = 0;
	size_t capacity_ = 0;
};

template<typename Type>
_NODISCARD bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& other)  noexcept
{
	return std::equal(lhs.cbegin(), lhs.cend(), other.cbegin());
}

template<typename Type>
_NODISCARD bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& other)  noexcept
{
	return !(lhs == other);
}

template<typename Type>
_NODISCARD bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& other)  noexcept
{
	return std::lexicographical_compare(lhs.cbegin(), lhs.cend(), other.cbegin(), other.cend());
}

template<typename Type>
_NODISCARD bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& other)  noexcept
{
	return other < lhs;
}

template<typename Type>
_NODISCARD bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& other)  noexcept
{
	return lhs == other || lhs < other;
}

template<typename Type>
_NODISCARD bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& other)  noexcept
{
	return lhs == other || lhs > other;
}

//template<typename Type>
//void swap(SimpleVector<Type>& lhs, SimpleVector<Type>& rhs) noexcept
//{
//	return lhs.swap(rhs);
//}

template<typename Type>
void swap(ArrayPtr<Type>& lhs, ArrayPtr<Type>& rhs) noexcept
{
	return lhs.swap(rhs);
}

class SimpleVectorTest
{
public:
	static void Test1() {
		using namespace std;
		// Инициализация конструктором по умолчанию
		{
			SimpleVector<int> v;
			assert(v.GetSize() == 0u);
			assert(v.IsEmpty());
			assert(v.GetCapacity() == 0u);
		}

		// Инициализация вектора указанного размера
		{
			SimpleVector<int> v2();
			SimpleVector<int> v(5);
			assert(v.GetSize() == 5u);
			assert(v.GetCapacity() == 5u);
			assert(!v.IsEmpty());
			for (size_t i = 0; i < v.GetSize(); ++i) {
				assert(v[i] == 0);
			}
		}

		// Инициализация вектора, заполненного заданным значением
		{
			SimpleVector<int> v(3, 42);
			assert(v.GetSize() == 3);
			assert(v.GetCapacity() == 3);
			for (size_t i = 0; i < v.GetSize(); ++i) {
				assert(v[i] == 42);
			}
		}

		// Инициализация вектора при помощи initializer_list
		{
			SimpleVector<int> v{ 1, 2, 3 };
			assert(v.GetSize() == 3);
			assert(v.GetCapacity() == 3);
			assert(v[2] == 3);
		}

		// Доступ к элементам при помощи At
		{
			SimpleVector<int> v(3);
			assert(&v.At(2) == &v[2]);
			try {
				v.At(3);
				assert(false);  // Ожидается выбрасывание исключения
			}
			catch (const std::out_of_range&) {
			}
			catch (...) {
				assert(false);  // Не ожидается исключение, отличное от out_of_range
			}
		}

		// Очистка вектора
		{
			SimpleVector<int> v(10);
			const size_t old_capacity = v.GetCapacity();
			v.Clear();
			assert(v.GetSize() == 0);
			assert(v.GetCapacity() == old_capacity);
		}

		// Изменение размера
		{
			SimpleVector<int> v(3);
			v[2] = 17;
			v.Resize(7);
			assert(v.GetSize() == 7);
			assert(v.GetCapacity() >= v.GetSize());
			assert(v[2] == 17);
			assert(v[3] == 0);
		}
		{
			SimpleVector<int> v(3);
			v[0] = 42;
			v[1] = 55;
			const size_t old_capacity = v.GetCapacity();
			v.Resize(2);
			assert(v.GetSize() == 2);
			assert(v.GetCapacity() == old_capacity);
			assert(v[0] == 42);
			assert(v[1] == 55);
		}
		{
			const size_t old_size = 3;
			SimpleVector<int> v(3);
			v.Resize(old_size + 5);
			v[3] = 42;
			v.Resize(old_size);
			v.Resize(old_size + 2);
			assert(v[3] == 0);
		}

		// Итерирование по SimpleVector
		{
			// Пустой вектор
			{
				SimpleVector<int> v;
				assert(v.begin() == nullptr);
				assert(v.end() == nullptr);
			}

			// Непустой вектор
			{
				SimpleVector<int> v(10, 42);
				assert(v.begin());
				assert(*v.begin() == 42);
				assert(v.end() == v.begin() + v.GetSize());
			}
		}
	}

	static void Test2() {
	     //PushBack
	    {
	        SimpleVector<int> v(1);
	        v.PushBack(42);
	        assert(v.GetSize() == 2);
	        assert(v.GetCapacity() >= v.GetSize());
	        assert(v[0] == 0);
	        assert(v[1] == 42);
	    }

	     //Если хватает места, PushBack не увеличивает Capacity
	    {
	        SimpleVector<int> v(2);
	        v.Resize(1);
	        const size_t old_capacity = v.GetCapacity();
	        v.PushBack(123);
	        assert(v.GetSize() == 2);
	        assert(v.GetCapacity() == old_capacity);
	    }

	     //PopBack
	    {
	        SimpleVector<int> v{ 0, 1, 2, 3 };
	        const size_t old_capacity = v.GetCapacity();
	        const auto old_begin = v.begin();
	        v.PopBack();
	        assert(v.GetCapacity() == old_capacity);
	        assert(v.begin() == old_begin);
	        assert((v == SimpleVector<int>{0, 1, 2}));
	    }

	     //Конструктор копирования
	    {
	        SimpleVector<int> numbers{ 1, 2 };
	        auto numbers_copy(numbers);
	        assert(&numbers_copy[0] != &numbers[0]);
	        assert(numbers_copy.GetSize() == numbers.GetSize());
	        for (size_t i = 0; i < numbers.GetSize(); ++i) {
	            assert(numbers_copy[i] == numbers[i]);
	            assert(&numbers_copy[i] != &numbers[i]);
	        }
	    }

	     //Сравнение
	    {
	        assert((SimpleVector{ 1, 2, 3 } == SimpleVector{ 1, 2, 3 }));
	        assert((SimpleVector{ 1, 2, 3 } != SimpleVector{ 1, 2, 2 }));

	        assert((SimpleVector{ 1, 2, 3 } < SimpleVector{ 1, 2, 3, 1 }));
	        assert((SimpleVector{ 1, 2, 3 } > SimpleVector{ 1, 2, 2, 1 }));

	        assert((SimpleVector{ 1, 2, 3 } >= SimpleVector{ 1, 2, 3 }));
	        assert((SimpleVector{ 1, 2, 4 } >= SimpleVector{ 1, 2, 3 }));
	        assert((SimpleVector{ 1, 2, 3 } <= SimpleVector{ 1, 2, 3 }));
	        assert((SimpleVector{ 1, 2, 3 } <= SimpleVector{ 1, 2, 4 }));
	    }

	     //Обмен значений векторов
	    {
	        SimpleVector<int> v1{ 42, 666 };
	        SimpleVector<int> v2;
	        v2.PushBack(0);
	        v2.PushBack(1);
	        v2.PushBack(2);
	        const int* const begin1 = &v1[0];
	        const int* const begin2 = &v2[0];

	        const size_t capacity1 = v1.GetCapacity();
	        const size_t capacity2 = v2.GetCapacity();

	        const size_t size1 = v1.GetSize();
	        const size_t size2 = v2.GetSize();

	        static_assert(noexcept(v1.swap(v2)));
	        v1.swap(v2);
	        assert(&v2[0] == begin1);
	        assert(&v1[0] == begin2);
	        assert(v1.GetSize() == size2);
	        assert(v2.GetSize() == size1);
	        assert(v1.GetCapacity() == capacity2);
	        assert(v2.GetCapacity() == capacity1);
	    }

	     //Присваивание
	    {
	        SimpleVector<int> src_vector{ 1, 2, 3, 4 };
	        SimpleVector<int> dst_vector{ 1, 2, 3, 4, 5, 6 };
	        dst_vector = src_vector;
	        assert(dst_vector == src_vector);
	    }

	    // Вставка элементов
	    {
	        SimpleVector<int> v{ 1, 2, 3, 4 };
	        v.Insert(v.begin() + 2, 42);
	        assert((v == SimpleVector<int>{1, 2, 42, 3, 4}));
	    }

	    // Удаление элементов
	    {
	        SimpleVector<int> v{ 1, 2, 3, 4 };
	        v.Erase(v.cbegin() + 2);
	        assert((v == SimpleVector<int>{1, 2, 4}));
	    }
	}

	static void TestReserveConstructor() {
		using namespace std;
		cout << "TestReserveConstructor"s << endl;
		SimpleVector<int> v(Reserve(5));
		assert(v.GetCapacity() == 5);
		assert(v.IsEmpty());
		cout << "Done!"s << endl;
	}

	static void TestReserveMethod() {
		using namespace std;
		cout << "TestReserveMethod"s << endl;
		SimpleVector<int> v;
		// зарезервируем 5 мест в векторе
		v.Reserve(5);
		assert(v.GetCapacity() == 5);
		assert(v.IsEmpty());

		// попытаемся уменьшить capacity до 1
		v.Reserve(1);
		// capacity должно остаться прежним
		assert(v.GetCapacity() == 5);
		// поместим 10 элементов в вектор
		for (int i = 0; i < 10; ++i) {
			v.PushBack(i);
		}
		assert(v.GetSize() == 10);
		// увеличим capacity до 100
		v.Reserve(100);
		// проверим, что размер не поменялся
		assert(v.GetSize() == 10);
		assert(v.GetCapacity() == 100);
		// проверим, что элементы на месте
		for (int i = 0; i < 10; ++i) {
			assert(v[i] == i);
		}
		cout << "Done!"s << endl;
	}
	
	class X {
	public:
		X()
			: X(5) {
		}
		X(size_t num)
			: x_(num) {
		}
		X(const X& other) = delete;
		X& operator=(const X& other) = delete;
		X(X&& other) {
			x_ = std::exchange(other.x_, 0);
		}
		X& operator=(X&& other) {
			x_ = std::exchange(other.x_, 0);
			return *this;
		}
		size_t GetX() const {
			return x_;
		}

	private:
		size_t x_;
	};

	static SimpleVector<int> GenerateVector(size_t size) {
		using namespace std;
		SimpleVector<int> v(size);
		iota(v.begin(), v.end(), 1);
		return v;
	}

	static void TestTemporaryObjConstructor() {
		using namespace std;
		const size_t size = 1000000;
		cout << "Test with temporary object, copy elision" << endl;
		SimpleVector<int> moved_vector(GenerateVector(size));
		assert(moved_vector.GetSize() == size);
		cout << "Done!" << endl << endl;
	}

	static void TestTemporaryObjOperator() {
		using namespace std;
		const size_t size = 1000000;
		cout << "Test with temporary object, operator=" << endl;
		SimpleVector<int> moved_vector;
		assert(moved_vector.GetSize() == 0);
		moved_vector = GenerateVector(size);
		assert(moved_vector.GetSize() == size);
		cout << "Done!" << endl << endl;
	}

	static void TestNamedMoveConstructor() {
		using namespace std;
		const size_t size = 1000000;
		cout << "Test with named object, move constructor" << endl;
		SimpleVector<int> vector_to_move(GenerateVector(size));
		assert(vector_to_move.GetSize() == size);

		SimpleVector<int> moved_vector(move(vector_to_move));
		assert(moved_vector.GetSize() == size);
		assert(vector_to_move.GetSize() == 0);
		cout << "Done!" << endl << endl;
	}

	static void TestNamedMoveOperator() {
		using namespace std;
		const size_t size = 1000000;
		cout << "Test with named object, operator=" << endl;
		SimpleVector<int> vector_to_move(GenerateVector(size));
		assert(vector_to_move.GetSize() == size);

		SimpleVector<int> moved_vector = move(vector_to_move);
		assert(moved_vector.GetSize() == size);
		assert(vector_to_move.GetSize() == 0);
		cout << "Done!" << endl << endl;
	}

	static void TestNoncopiableMoveConstructor() {
		using namespace std;
		const size_t size = 5;
		cout << "Test noncopiable object, move constructor" << endl;
		SimpleVector<X> vector_to_move;
		for (size_t i = 0; i < size; ++i) {
			vector_to_move.PushBack(X(i));
		}

		SimpleVector<X> moved_vector = move(vector_to_move);
		assert(moved_vector.GetSize() == size);
		assert(vector_to_move.GetSize() == 0);

		for (size_t i = 0; i < size; ++i) {
			assert(moved_vector[i].GetX() == i);
		}
		cout << "Done!" << endl << endl;
	}

	static void TestNoncopiablePushBack() {
		using namespace std;
		const size_t size = 5;
		cout << "Test noncopiable push back" << endl;
		SimpleVector<X> v;
		for (size_t i = 0; i < size; ++i) {
			v.PushBack(X(i));
		}

		assert(v.GetSize() == size);

		for (size_t i = 0; i < size; ++i) {
			assert(v[i].GetX() == i);
		}
		cout << "Done!" << endl << endl;
	}

	static void TestNoncopiableInsert() {
		using namespace std;
		const size_t size = 5;
		cout << "Test noncopiable insert" << endl;
		SimpleVector<X> v;
		for (size_t i = 0; i < size; ++i) {
			v.PushBack(X(i));
		}

		// в начало
		v.Insert(v.begin(), X(size + 1));
		assert(v.GetSize() == size + 1);
		assert(v.begin()->GetX() == size + 1);
		// в конец
		v.Insert(v.end(), X(size + 2));
		assert(v.GetSize() == size + 2);
		assert((v.end() - 1)->GetX() == size + 2);
		// в середину
		v.Insert(v.begin() + 3, X(size + 3));
		assert(v.GetSize() == size + 3);
		assert((v.begin() + 3)->GetX() == size + 3);
		cout << "Done!" << endl << endl;
	}

	static void TestNoncopiableErase() {
		using namespace std;
		const size_t size = 3;
		cout << "Test noncopiable erase" << endl;
		SimpleVector<X> v;
		for (size_t i = 0; i < size; ++i) {
			v.PushBack(X(i));
		}

		auto it = v.Erase(v.begin());
		assert(it->GetX() == 1);
		cout << "Done!" << endl << endl;
	}

};
