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

    SimpleVector(const SimpleVector &other) {
        SimpleVector tmp(other.size_);
        if (!other.IsEmpty()) {
            tmp.size_ = other.size_;
            tmp.capacity_ = other.capacity_;
            std::copy(other.begin(), other.end(), tmp.begin());
        }
        swap(tmp);
    }

    SimpleVector(SimpleVector &&other) noexcept {
        data.swap(other.data);
        //size_ = std::move(other.size_);
        //capacity_ = std::move(other.capacity_);
        other.size_ = std::exchange(size_, other.size_);
        other.capacity_ = std::exchange(capacity_, other.capacity_);
    }

    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size) {
        if (size > 0) {
            size_ = size;
            capacity_ = size;
            ArrayPtr<Type> tmp(size);
            data.swap(tmp);
            std::fill(begin(), end(), Type());
        }
    }

    explicit SimpleVector(ReserveProxyObj obj) {
        if (obj.capacity_ > 0) {
            capacity_ = obj.capacity_;
            ArrayPtr<Type> tmp(obj.capacity_);
            data.swap(tmp);
            std::fill(begin(), end(), Type());
        }
    }

    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type &value) {
        if (size > 0) {
            size_ = size;
            capacity_ = size;
            ArrayPtr<Type> tmp(size);
            data.swap(tmp);
            std::fill(begin(), end(), value);
        }
    }

    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) {
        size_ = init.size();
        capacity_ = size_;
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
        if (index >= size_) throw std::out_of_range("");
        return data[index];
    }

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type &At(size_t index) const {
        if (index >= size_) throw std::out_of_range("");
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
            for (auto it = end(); it != data.Get() + capacity_; ++it) {
                *it = std::move(Type());
            }
            //std::fill(end(), data.Get() + capacity_, std::move(Type()));
        } else {
            //while (new_size > capacity_) capacity_ *= 2;
            capacity_ = new_size;
            ArrayPtr<Type> tmp(capacity_);
            //std::fill(tmp.Get(), tmp.Get() + capacity_, std::move(Type()));
            for (auto it = tmp.Get(); it != tmp.Get() + capacity_; ++it) {
                *it = std::move(Type());
            }
            std::move(begin(), end(), tmp.Get());
            data.swap(tmp);
            size_ = new_size;
        }
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            capacity_ = new_capacity;
            ArrayPtr<Type> tmp(capacity_);
            std::fill(tmp.Get(), tmp.Get() + capacity_, Type());
            std::copy(begin(), end(), tmp.Get());
            data.swap(tmp);
        }
    }

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type &item) {
        if (size_ < capacity_) {
            data[size_] = item;
        } else {
            if (capacity_ == 0) capacity_ = 1;
            else capacity_ *= 2;
            ArrayPtr<Type> tmp(capacity_);
            std::copy(begin(), end(), tmp.Get());
            tmp[size_] = item;
            data.swap(tmp);
        }
        ++size_;
    }

    void PushBack(Type &&item) {
        if (size_ < capacity_) {
            data[size_] = std::move(item);
        } else {
            if (capacity_ == 0) capacity_ = 1;
            else capacity_ *= 2;
            ArrayPtr<Type> tmp(capacity_);
            std::move(begin(), end(), tmp.Get());
            tmp[size_] = std::move(item);
            data.swap(tmp);
        }
        ++size_;
    }

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(Iterator pos, const Type &value) {
        auto dist = std::distance(begin(), pos);
        if (size_ < capacity_) {
            std::copy_backward(pos, end(), pos + 1);
            *pos = value;
        } else {
            if (capacity_ == 0) capacity_ = 1;
            else capacity_ *= 2;
            ArrayPtr<Type> tmp(capacity_);
            std::copy(begin(), pos, tmp.Get());
            std::copy(pos, end(), tmp.Get() + dist + 1);
            tmp[dist] = value;
            data.swap(tmp);
        }
        ++size_;
        return Iterator(begin() + dist);
    }

    Iterator Insert(Iterator pos, Type &&value) {
        auto dist = std::distance(begin(), pos);
        if (size_ < capacity_) {
            std::move_backward(pos, end(), end() + 1);
            *pos = std::move(value);
        } else {
            if (capacity_ == 0) capacity_ = 1;
            else capacity_ *= 2;
            ArrayPtr<Type> tmp(capacity_);
            std::move(begin(), pos, tmp.Get());
            std::move(pos, end(), tmp.Get() + dist + 1);
            tmp[dist] = std::move(value);
            data.swap(tmp);
        }
        ++size_;
        return Iterator(begin() + dist);
    }

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        if (!IsEmpty()) --size_;
    }

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(Iterator pos) {
        auto dist = std::distance(begin(), pos);
        ArrayPtr<Type> tmp(size_ - 1);
        std::move(begin(), pos, tmp.Get());
        std::move(pos + 1, end(), tmp.Get() + dist);
        data.swap(tmp);
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
    return !(operator==(lhs, rhs));
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