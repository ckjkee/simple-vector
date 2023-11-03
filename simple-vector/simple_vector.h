#pragma once
 
#include <cassert>
#include <stdexcept>
#include <initializer_list>

#include "array_ptr.h"
 
class ReserveProxyObj {
public:
 
    explicit ReserveProxyObj(size_t new_capacity) {
        new_capacity_ = new_capacity;        
    }
 
    size_t GetCapacity() {
        return new_capacity_;
    }
 
private:
    size_t new_capacity_ = 0;
 
};
 
ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}
 
template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;
 
    SimpleVector() noexcept = default;
    SimpleVector(SimpleVector&& other) :SimpleVector(other.size_) {
        std::move(other.begin(), other.end(), begin());
        other.size_ = 0;
    }
    SimpleVector& operator=(SimpleVector&& rhs) {
        if(rhs.ptr_.Get() != ptr_.Get()) {
            ArrayPtr<Type> ptr(rhs.size_);
            std::move(rhs.begin(), rhs.end(), ptr.Get());
            ptr_.swap(ptr);
            size_ = rhs.size_;
            capacity_ = rhs.capacity_;
            rhs.size_ = 0;
        }
        return *this;
    }
 
    explicit SimpleVector(size_t size) : ptr_(size), size_(size), capacity_(size) {        
        Fill(begin(), end());
    }
 
    SimpleVector(size_t size, const Type& value) : SimpleVector(size) {
        std::fill(begin(), end(), value);
    }
 
    SimpleVector(std::initializer_list<Type> init) : SimpleVector(init.size()) {
        std::copy(init.begin(), init.end(), begin());
    }
 
    SimpleVector(const SimpleVector& other) : SimpleVector(other.size_) {
        std::copy(other.begin(), other.end(), begin());
    }
 
    SimpleVector& operator=(const SimpleVector& rhs) {
        if(rhs.ptr_.Get() != ptr_.Get()) {
            ArrayPtr<Type> ptr(rhs.size_);
            std::copy(rhs.begin(), rhs.end(), ptr.Get());
            ptr_.swap(ptr);
            size_ = rhs.size_;
            capacity_ = rhs.capacity_;
        }
        return *this;
    }
 
    SimpleVector(ReserveProxyObj new_capacity) {
        Reserve(new_capacity.GetCapacity());
    }
 
    void Reserve(size_t new_capacity) {
        if(new_capacity > capacity_) {
            ArrayPtr<Type> ptr(new_capacity);
            std::move(begin(), end(), ptr.Get());
            ptr_.swap(ptr);
            capacity_ = new_capacity;
        }
    }
 
    void PushBack(const Type& item) {
        if(size_ < capacity_) {
            ptr_[size_] = item;
            ++size_;
        } else {
            capacity_ = capacity_ ? capacity_ * 2 : 1;
            ArrayPtr<Type> ptr(capacity_);
            std::move(begin(), end(), ptr.Get());
            ptr_.swap(ptr);
            ptr_[size_] = item;
            ++size_;
        }
    }
 
    void PushBack(Type&& item) {
        if(size_ < capacity_) {
            ptr_[size_] = std::move(item);
            ++size_;
        } else {
            capacity_ = capacity_ ? capacity_ * 2 : 1;
            ArrayPtr<Type> ptr(capacity_);
            std::move(begin(), end(), ptr.Get());
            ptr_.swap(ptr);
            ptr_[size_] = std::move(item);
             ++size_;
        }
    }
 
    Iterator Insert(ConstIterator pos, const Type& value) {
        if(size_ < capacity_) {
            std::move(Iterator(pos), end(), Iterator(pos) + 1);
            *(Iterator(pos)) = value;
            ++size_;
            return Iterator(pos);
        } else {
            capacity_ = capacity_ ? capacity_ * 2 : 1;
            ArrayPtr<Type> ptr(capacity_);
            Iterator new_pos = std::move(begin(), Iterator(pos), ptr.Get());
            *new_pos = value;
            std::move(Iterator(pos), end(), new_pos + 1);
            ptr_.swap(ptr);
            ++size_;
            return new_pos;
        }
    }
 
    Iterator Insert(ConstIterator pos, Type&& value) {
        if(size_ < capacity_) {
            std::move(Iterator(pos), end(), Iterator(pos) + 1);
            *(Iterator(pos)) = std::move(value);
            ++size_;
            return Iterator(pos);
        } else {
            capacity_ = capacity_ ? capacity_ * 2 : 1;
            ArrayPtr<Type> ptr(capacity_);
            Iterator new_pos = std::move(begin(), Iterator(pos), ptr.Get());
            *new_pos = std::move(value);
            std::move(Iterator(pos), end(), new_pos + 1);
            ptr_.swap(ptr);
            ++size_; 
            return new_pos;
            }
    }
 
    void PopBack() noexcept {
        if(size_ > 0) {
            --size_;
        }
    }
 
    Iterator Erase(ConstIterator pos) {
        std::move(Iterator(pos) + 1, end(), Iterator(pos));
        --size_;
        return Iterator(pos);
    }
 
    void swap(SimpleVector& other) noexcept {
        ptr_.swap(other.ptr_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
 
    size_t GetSize() const noexcept {
        return size_;
    }
 
    size_t GetCapacity() const noexcept {
        return capacity_;
    }
 
    bool IsEmpty() const noexcept {
        return size_ == 0;
    }
 
    Type& operator[](size_t index) noexcept {
        return ptr_[index];
    }
 
    const Type& operator[](size_t index) const noexcept {
        return ptr_[index];
    }
 
    Type& At(size_t index) {
        if(index >= size_) {
            throw std::out_of_range("element is not in range");
        }
        return ptr_[index];
    }
 
    const Type& At(size_t index) const {
        if(index >= size_) {
            throw std::out_of_range("element is not in range");
        }
        return ptr_[index];
    }
 
    void Clear() noexcept {
        size_ = 0;
    }
 
  void Resize(size_t new_size) {
        if(new_size <= size_) {
            size_ = new_size;
        } else if(new_size > size_ && new_size <= capacity_) {
            Fill(begin() + size_, begin() + new_size);
            size_ = new_size;
        } else if(new_size > capacity_) {
            size_t new_capacity = std::max(new_size, capacity_*2);
            ArrayPtr<Type> ptr(new_capacity);
            std::move(begin(), end(), ptr.Get());
            ptr_.swap(ptr);
            Fill(begin() + size_, begin() + new_size);
            size_ = new_size;
            capacity_ = new_capacity;
        }
    }
 
    Iterator begin() noexcept {
        return ptr_.Get();
    }
 
    Iterator end() noexcept {
        return ptr_.Get() + size_;
    }
 
    ConstIterator begin() const noexcept {
        return ptr_.Get();
    }
 
    ConstIterator end() const noexcept {
        return ptr_.Get() + size_;
    }

    ConstIterator cbegin() const noexcept {
        return begin();
    }
 
    ConstIterator cend() const noexcept {
        return end();
    }
private:
    ArrayPtr<Type> ptr_;
    size_t size_ = 0;
    size_t capacity_ = 0;
 
    void Fill(Iterator start, Iterator end) {
        while (start != end) {
            *start = Type{};
            ++start;
        }
    }
 
};
 
template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    if(&lhs == &rhs) {
        return true;
    }
    if(lhs.GetSize() != rhs.GetSize()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}
 
template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}
 
template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
 
template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}
 
template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}
 
template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}
