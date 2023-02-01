#pragma once

#include <bits/stdc++.h>
#include "array_ptr.h"

class ReserveProxyObj {
public:
    explicit ReserveProxyObj(size_t capacity) {
        capacity_ = capacity;
    }

    size_t capacity_;
};

ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template<typename Type>
class SimpleVector {
public:
    using Iterator = Type *;
    using ConstIterator = const Type *;

    SimpleVector() noexcept = default;

    SimpleVector(const SimpleVector &other) : size_(other.size_), capacity_(other.capacity_) {
        ArrayPtr<Type> tmp(other.size_);
        std::copy(other.begin(), other.end(), tmp.Get());
        data.swap(tmp);
    }

    SimpleVector(SimpleVector &&other) noexcept {
        swap(other);
    }

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) : size_(size), capacity_(size) {
        assert(size > 0);
        ArrayPtr<Type> tmp(size);
        data.swap(tmp);
        std::fill(begin(), end(), Type());
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type &value) : size_(size), capacity_(size) {
        assert(size > 0);
        ArrayPtr<Type> tmp(size);
        data.swap(tmp);
        std::fill(begin(), end(), value);
    }

    explicit SimpleVector(ReserveProxyObj obj) {
        Reserve(obj.capacity_);
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) : size_(init.size()), capacity_(init.size()) {
        ArrayPtr<Type> tmp(size_);
        data.swap(tmp);
        std::copy(init.begin(), init.end(), begin());
    }

    SimpleVector &operator=(const SimpleVector &rhs) {
        if (this != &rhs) {
            auto rhs_copy(rhs);
            swap(rhs_copy);
        }
        return *this;
    }

    SimpleVector &operator=(SimpleVector &&rhs) noexcept {
        data.swap(rhs.data);
        size_ = std::move(rhs.size_);
        capacity_ = std::move(rhs.capacity_);
        return *this;
    }

    // Возвращает ссылку на элемент с индексом index
    Type &operator[](size_t index) noexcept {
        return data[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    const Type &operator[](size_t index) const noexcept {
        return data[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type &At(size_t index) {
        if (index >= size_) throw std::out_of_range("Index out of range.");
        return data[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type &At(size_t index) const {
        if (index >= size_) throw std::out_of_range("Index out of range.");
        return data[index];
    }

    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }

    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
        } else if (new_size <= capacity_) {
            std::generate(end(), data.Get() + capacity_, []() { return Type(); });
        } else {
            //while (new_size > capacity_) capacity_ *= 2;
            Reserve(new_size);
            std::generate(end(), data.Get() + capacity_, []() { return Type(); });
            size_ = new_size;
        }
    }

    void Reserve(size_t new_capacity) {
        assert(new_capacity >= 0);
        if (new_capacity > capacity_) {
            ArrayPtr<Type> tmp(new_capacity);
            std::move(begin(), end(), tmp.Get());
            data.swap(tmp);
            capacity_ = new_capacity;
        }
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type &item) {
        if (size_ < capacity_) {
            data[size_] = item;
        } else {
            if (capacity_ == 0) Reserve(1);
            else Reserve(capacity_ * 2);
            data[size_] = item;
        }
        ++size_;
    }

    void PushBack(Type &&item) {
        if (size_ < capacity_) {
            data[size_] = std::move(item);
        } else {
            if (capacity_ == 0) Reserve(1);
            else Reserve(capacity_ * 2);
            data[size_] = std::move(item);
        }
        ++size_;
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(Iterator pos, const Type &value) {
        auto dist = std::distance(begin(), pos);
        *Expand(pos, dist) = value;
        ++size_;
        return Iterator(begin() + dist);
    }

    Iterator Insert(Iterator pos, Type &&value) {
        auto dist = std::distance(begin(), pos);
        *Expand(pos, dist) = std::move(value);
        ++size_;
        return Iterator(begin() + dist);
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert(!IsEmpty());
        --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(Iterator pos) {
        assert(begin() <= pos && pos < end());
        auto dist = std::distance(begin(), pos);
        std::move(pos + 1, end(), pos);
        --size_;
        return Iterator(begin() + dist);
    }

    // Обменивает значение с другим вектором
    void swap(SimpleVector &other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        data.swap(other.data);
    }

    // Возвращает количество элементов в массиве
    [[nodiscard]] size_t GetSize() const noexcept {
        return size_;
    }

    // Возвращает вместимость массива
    [[nodiscard]] size_t GetCapacity() const noexcept {
        return capacity_;
    }

    // Сообщает, пустой ли массив
    [[nodiscard]] bool IsEmpty() const noexcept {
        return size_ == 0;
    }

    // Возвращает итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator begin() noexcept {
        return data.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    Iterator end() noexcept {
        return data.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator begin() const noexcept {
        return data.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator end() const noexcept {
        return data.Get() + size_;
    }

    // Возвращает константный итератор на начало массива
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cbegin() const noexcept {
        return data.Get();
    }

    // Возвращает итератор на элемент, следующий за последним
    // Для пустого массива может быть равен (или не равен) nullptr
    ConstIterator cend() const noexcept {
        return data.Get() + size_;
    }

private:
    Iterator Expand(Iterator pos, size_t distance) {
        if (size_ < capacity_) {
            std::move(pos, end(), pos + 1);
        } else {
            size_t new_capacity_ = capacity_;
            if (capacity_ == 0) new_capacity_ = 1;
            else new_capacity_ *= 2;
            ArrayPtr<Type> tmp(new_capacity_);
            std::move(begin(), pos, tmp.Get());
            std::move(pos, end(), tmp.Get() + distance + 1);
            data.swap(tmp);
            capacity_ = new_capacity_;
        }
        return Iterator(begin() + distance);
    }

private:
    ArrayPtr<Type> data;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

template<typename Type>
inline bool operator==(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename Type>
inline bool operator!=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
    return !(lhs == rhs);
}

template<typename Type>
inline bool operator<(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                        rhs.begin(), rhs.end());
}

template<typename Type>
inline bool operator<=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
    return (operator==(lhs, rhs)) || (operator<(lhs, rhs));
}

template<typename Type>
inline bool operator>(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
    return !(operator<=(lhs, rhs));
}

template<typename Type>
inline bool operator>=(const SimpleVector<Type> &lhs, const SimpleVector<Type> &rhs) {
    return (operator==(lhs, rhs)) || (operator>(lhs, rhs));
}