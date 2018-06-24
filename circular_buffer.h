//
// Created by egor on 23.06.18.
//

#ifndef BUFFER_CYCLE_DEQUEU_H
#define BUFFER_CYCLE_DEQUEU_H


#include <cassert>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

template <typename T>
struct circular_buffer{
private:
    template <typename V>
    struct my_iterator: public std::iterator<std::random_access_iterator_tag, T> {
    public:

        my_iterator(size_t pos, size_t capacity, size_t left, size_t right, T* array)
                : pos(pos), capacity(capacity), left(left), right(right), array(array) {};
        my_iterator() = default;
        my_iterator(my_iterator const&) = default;
        my_iterator& operator=(my_iterator const&) = default;

        template <typename U>
        my_iterator(my_iterator<U> const&other, typename std::enable_if<std::is_same<U const, V>::value && std::is_const<V>::value>::type* = nullptr)
                : pos(other.pos), capacity(other.capacity), left(other.left), right(other.right), array(other.array) {}


        T& operator*() const;
        T* operator->() const;
        my_iterator& operator++();
        my_iterator operator++(int);

        my_iterator& operator--();
        my_iterator operator--(int);


        my_iterator& operator+=(const int offset) {
            pos = (pos + offset) % capacity;
            return *this;
        };
        friend my_iterator operator+(my_iterator other, const int offset) {
            return other += offset;
        };
        my_iterator& operator-=(const int offset) {
            pos = (pos - offset) % capacity;
            return *this;
        };
        friend my_iterator operator-(my_iterator other, const int offset) {
            return other -= offset;
        };
        friend ptrdiff_t operator-(my_iterator const &a, my_iterator const &b) {
            ptrdiff_t dist_a = distance(a);
            ptrdiff_t dist_b = distance(b);
            return dist_a - dist_b;
        };

        friend bool operator==(my_iterator const &a, my_iterator const &b) {
            return compare(a, b) == 0;
        }
        friend bool operator!=(my_iterator const &a, my_iterator const &b) {
            return compare(a, b) != 0;
        }
        friend bool operator<(my_iterator const &a, my_iterator const &b) {
            return compare(a, b) == - 1;
        };
        friend bool operator<=(my_iterator const &a, my_iterator const &b) {
            int cmp = compare(a, b);
            return cmp == -1 || cmp == 0;
        };
        friend bool operator>(my_iterator const &a, my_iterator const &b) {
            return compare(a, b) == 1;
        };
        friend bool operator>=(my_iterator const &a, my_iterator const &b) {
            int cmp = compare(a, b);
            return cmp == 0 || cmp == 1;
        };
    private:
        size_t pos, capacity;
        size_t left, right;
        T *array;
        friend struct circular_buffer;
        static int compare(my_iterator const &a, my_iterator const &b) {
            size_t dist_a = distance(a);
            size_t dist_b = distance(b);
            if (dist_a < dist_b) {
                return -1;
            }
            if (dist_a == dist_b) {
                return 0;
            }
            return 1;
        }
        static size_t distance(my_iterator const &a) {
            if (a.right >= a.left) {
                return a.pos - a.left;
            }
            return a.capacity - 1 - a.left + a.pos;
        }
    };

public:
    using iterator = my_iterator<T>;
    using const_iterator = my_iterator<T const>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;


    circular_buffer();
    explicit circular_buffer(size_t size);
    circular_buffer(circular_buffer const &other);
    circular_buffer& operator=(circular_buffer const &other);
    ~circular_buffer();


    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;

    reverse_iterator rend();
    const_reverse_iterator rend() const;

    void push_back(T const &value);
    void pop_back();
    T& back();
    T const& back() const;


    void push_front(T const &value);
    void pop_front();
    T& front();
    T const& front() const;

    iterator insert(const_iterator pos, T const &value);
    iterator erase(const_iterator pos);
    T& operator[](size_t pos);
    bool empty();
    void clear();
    size_t size();

    void swap(circular_buffer &other);

private:
    void ensure_capacity();
    size_t prev(size_t x);
    size_t next(size_t x);
    size_t abs(ptrdiff_t x);

    size_t size_, capacity;
    size_t left, right;
    T *array;

};

template<typename T>
template<typename V>
T &circular_buffer<T>::my_iterator<V>::operator*() const {
    return array[pos];
}

template<typename T>
template<typename V>
circular_buffer<T>::my_iterator<V> &circular_buffer<T>::my_iterator<V>::operator++() {
    pos++;
    if (pos == capacity) {
        pos = 0;
    }
    return *this;
}

template<typename T>
template<typename V>
circular_buffer<T>::my_iterator<V> circular_buffer<T>::my_iterator<V>::operator++(int) {
    my_iterator tmp(pos, capacity, left, right, array);
    ++*this;
    return tmp;
}

template<typename T>
template<typename V>
circular_buffer<T>::my_iterator<V> &circular_buffer<T>::my_iterator<V>::operator--() {
    if (pos == 0) {
        pos = capacity - 1;
    } else {
        pos--;
    }
    return *this;
}

template<typename T>
template<typename V>
circular_buffer<T>::my_iterator<V> circular_buffer<T>::my_iterator<V>::operator--(int) {
    my_iterator tmp(pos, capacity, left, right, array);
    --*this;
    return tmp;
}

template<typename T>
template<typename V>
T *circular_buffer<T>::my_iterator<V>::operator->() const {
    return array[pos];
}

template<typename T>
circular_buffer<T>::circular_buffer() : size_(0), capacity(0), left(0), right(0), array(nullptr) {}

template<typename T>
circular_buffer<T>::~circular_buffer() {
    for (size_t i = 0; i < size_; i++) {
        array[left].~T();
        left = next(left);
    }
    free(array);
}

template<typename T>
void circular_buffer<T>::push_back(const T &value) {
    if (size_ == capacity - 1 || capacity == 0) {
        ensure_capacity();
    }

    if (size_ == 0) {
        try{
            new (array + right) T(value);
        } catch (...) {
            size_ = 0;
            throw;
        }
        size_ = 1;
        return;
    }


    right = next(right);
    try {
        new (array + right) T(value);
        size_++;
    } catch (...) {
        right = prev(right);
        throw;
    }
}

template<typename T>
void circular_buffer<T>::push_front(const T &value) {
    if (size_ == capacity - 1 || capacity == 0) {
        ensure_capacity();
    }

    if (size_ == 0) {
        try{
            new (array + left) T(value);
        } catch (...) {
            size_ = 0;
            throw ;
        }
        size_ = 1;
        return;
    }
    left = prev(left);
    try {
        new (array + left) T(value);
        size_++;
    } catch (...) {
        left = next(left);
        throw;
    }
}

template<typename T>
T &circular_buffer<T>::operator[](size_t pos) {
    assert(pos < size_);
    return array[(pos + left)%capacity];
}

template<typename T>
size_t circular_buffer<T>::prev(size_t x) {
    if (x == 0) {
        x = capacity - 1;
    } else {
        x--;
    }
    return x;
}

template<typename T>
size_t circular_buffer<T>::next(size_t x) {
    if (x == capacity - 1) {
        x = 0;
    } else {
        x++;
    }
    return x;
}

template<typename T>
void circular_buffer<T>::ensure_capacity() {
    if (capacity == 0) {
        capacity = 2;
    }
    circular_buffer temp_buff = circular_buffer(2 * capacity - 1);
    for (size_t i = 0; i < size_; ++i) {
        temp_buff.push_back(operator[](i));
    }
    swap(temp_buff);
}

template<typename T>
void circular_buffer<T>::clear() {
    circular_buffer<T> tmp;
    swap(tmp);
}

template<typename T>
void circular_buffer<T>::pop_back() {
    assert(size_ > 0);
    size_--;
    array[right].~T();
    if (size_ == 0) {
        right = left;
    } else {
        right = prev(right);
    }
}

template<typename T>
void circular_buffer<T>::pop_front() {
    assert(size_ > 0);
    size_--;
    array[left].~T();
    if (size_ == 0)  {
        left = right;
    } else {
        left = next(left);
    }
}

template<typename T>
T &circular_buffer<T>::back() {
    return array[right];
}

template<typename T>
T const &circular_buffer<T>::back() const {
    return array[right];
}

template<typename T>
T &circular_buffer<T>::front() {
    return array[left];
}

template<typename T>
T const &circular_buffer<T>::front() const {
    return array[left];
}

template<typename T>
typename circular_buffer<T>::iterator circular_buffer<T>::begin() {
    return iterator(left, capacity, left, right, array);
}

template<typename T>
typename circular_buffer<T>::const_iterator circular_buffer<T>::begin() const {
    return const_iterator(left, capacity, left, right, array);
}

template<typename T>
typename circular_buffer<T>::iterator circular_buffer<T>::end() {
    if (size_ == 0) {
        return iterator(right, capacity, left, right, array);
    }
    return iterator(right + 1, capacity, left, right, array);
}

template<typename T>
typename circular_buffer<T>::const_iterator circular_buffer<T>::end() const {
    if (size_ == 0) {
        return const_iterator(right, capacity, left, right, array);
    }
    return const_iterator(right + 1, capacity, left, right, array);
}

template<typename T>
bool circular_buffer<T>::empty() {
    return (size_ == 0);
}


template<typename T>
size_t circular_buffer<T>::size() {
    return size_;
}

template<typename T>
typename circular_buffer<T>::reverse_iterator circular_buffer<T>::rbegin() {
    return reverse_iterator(end());
}

template<typename T>
typename circular_buffer<T>::reverse_iterator circular_buffer<T>::rend() {
    return reverse_iterator(begin());
}

template<typename T>
typename circular_buffer<T>::const_reverse_iterator circular_buffer<T>::rbegin() const {
    return const_reverse_iterator(end());
}

template<typename T>
typename circular_buffer<T>::const_reverse_iterator circular_buffer<T>::rend() const {
    return const_reverse_iterator(begin());
}

template<typename T>
circular_buffer<T>::circular_buffer(circular_buffer const &other) : circular_buffer() {
    for (auto it = other.begin(); it != other.end(); it++) {
        push_back(*it);
    }
}

template<typename T>
circular_buffer<T> &circular_buffer<T>::operator=(circular_buffer const &other) {
    circular_buffer<T> tmp(other);
    swap(tmp);
    return *this;
}

template<typename T>
void circular_buffer<T>::swap(circular_buffer &other) {
    std::swap(left, other.left);
    std::swap(right, other.right);
    std::swap(size_, other.size_);
    std::swap(capacity, other.capacity);
    std::swap(array, other.array);
}

template<typename T>
typename circular_buffer<T>::iterator circular_buffer<T>::insert(const_iterator it, const T &value) {
    size_t distance_left = abs(static_cast<ptrdiff_t>(it.pos) - static_cast<ptrdiff_t>(left));
    size_t distance_right = abs(static_cast<ptrdiff_t>(it.pos) - static_cast<ptrdiff_t>(right));
    if (size_ == capacity - 1) {
        ensure_capacity();
    }

    if (it.pos == left) {
        push_front(value);
        return iterator(left, capacity, left, right, array);
    }
    if (it.pos == next(right)) {
        push_back(value);
        return iterator(right, capacity, left, right, array);
    }
    size_t curr_pos;
    if (distance_left < distance_right) {
        curr_pos = left;
        push_front(array[left]);
        while(next(curr_pos) != it.pos) {
            array[curr_pos] = array[next(curr_pos)];
            curr_pos = next(curr_pos);
        }
        array[curr_pos] = value;
    } else  {
        curr_pos = right;
        push_back(array[right]);
        while(curr_pos != it.pos) {
            array[curr_pos] = array[prev(curr_pos)];
            curr_pos = prev(curr_pos);
        }
        array[curr_pos] = value;
    }

    return iterator(curr_pos, capacity, left, right, array);
}

template<typename T>
typename circular_buffer<T>::iterator circular_buffer<T>::erase(const_iterator it) {
    assert(size_ > 0);
    size_t distance_left = abs(static_cast<ptrdiff_t>(it.pos) - static_cast<ptrdiff_t>(left));
    size_t distance_right = abs(static_cast<ptrdiff_t>(it.pos) - static_cast<ptrdiff_t>(right));

    size_t curr_pos = it.pos;
    if (distance_left < distance_right) {
        size_t prev_pos = prev(it.pos);
        for (size_t i = 0; i < distance_left; ++i) {
            std::swap(array[prev_pos], array[curr_pos]);
            curr_pos = prev_pos;
            prev_pos = prev(prev_pos);
        }
        pop_front();
        return iterator(it.pos + 1, capacity, left, right, array);
    } else {
        size_t next_pos = next(it.pos);
        for (size_t i = 0; i < distance_right; ++i) {
            std::swap(array[next_pos], array[curr_pos]);
            curr_pos = next_pos;
            next_pos = next(next_pos);
        }
        pop_back();
        return iterator(it.pos, capacity, left, right, array);
    }
}

template<typename T>
size_t circular_buffer<T>::abs(ptrdiff_t x) {
    if (x < 0) {
        return static_cast<size_t>(-x);
    } else {
        return static_cast<size_t>(x);
    }
}

template<typename T>
circular_buffer<T>::circular_buffer(size_t capacity) : size_(0), capacity(capacity), left(0), right(0) {
    array = (T *)malloc(capacity * sizeof(T));
    if (array == nullptr) {
        throw std::bad_alloc();
    }
}


template <typename T>
void swap(circular_buffer<T>& a, circular_buffer<T>& b)
{
    a.swap(b);
}
#endif //BUFFER_CYCLE_DEQUEU_H
