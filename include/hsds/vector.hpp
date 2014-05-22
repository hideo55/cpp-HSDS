/*
 * vector.hpp
 *
 *  Created on: 2014/05/22
 *      Author: hide
 */

#if !defined(HSDS_VECTOR_HPP_)
#define HSDS_VECTOR_HPP_

#include <cstddef>
#include "scoped_array.hpp"
#include "exception.hpp"

#define HSDS_SIZE_MAX   ((size_t)~(size_t)0)

namespace hsds {

template<typename T>
class Vector {
public:
    Vector() :
            buf_(), objects_(NULL), const_objects_(NULL), size_(0), capacity_(0), fixed_(false) {
    }
    ~Vector() {
        if (objects_ != NULL) {
            for (std::size_t i = 0; i < size_; ++i) {
                objects_[i].~T();
            }
        }
    }

    void map(void *ptr, size_t size) {
        Vector temp;
        temp.map_(mapper);
        swap(temp);
    }

    void load(std::istream& is) {
        Vector temp;
        temp.read_(is);
        swap(temp);
    }

    void save(std::ostream& os) const {
        write_(os);
    }

    void push_back(const T &x) {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);
        HSDS_DEBUG_IF(size_ == max_size(), HSDS_SIZE_ERROR);
        reserve(size_ + 1);
        new (&objects_[size_]) T(x);
        ++size_;
    }

    void pop_back() {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);
        HSDS_DEBUG_IF(size_ == 0, HSDS_STATE_ERROR);
        objects_[--size_].~T();
    }

    // resize() assumes that T's placement new does not throw an exception.
    void resize(std::size_t size) {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);
        reserve(size);
        for (std::size_t i = size_; i < size; ++i) {
            new (&objects_[i]) T;
        }
        for (std::size_t i = size; i < size_; ++i) {
            objects_[i].~T();
        }
        size_ = size;
    }

    // resize() assumes that T's placement new does not throw an exception.
    void resize(std::size_t size, const T &x) {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);
        reserve(size);
        for (std::size_t i = size_; i < size; ++i) {
            new (&objects_[i]) T(x);
        }
        for (std::size_t i = size; i < size_; ++i) {
            objects_[i].~T();
        }
        size_ = size;
    }

    void reserve(std::size_t capacity) {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);
        if (capacity <= capacity_) {
            return;
        }
        HSDS_DEBUG_IF(capacity > max_size(), HSDS_SIZE_ERROR);
        std::size_t new_capacity = capacity;
        if (capacity_ > (capacity / 2)) {
            if (capacity_ > (max_size() / 2)) {
                new_capacity = max_size();
            } else {
                new_capacity = capacity_ * 2;
            }
        }
        realloc(new_capacity);
    }

    void shrink() {
        HSDS_THROW_IF(fixed_, HSDS_STATE_ERROR);
        if (size_ != capacity_) {
            realloc(size_);
        }
    }

    void fix() {
        HSDS_THROW_IF(fixed_, HSDS_STATE_ERROR);
        fixed_ = true;
    }

    const T *begin() const {
        return const_objects_;
    }
    const T *end() const {
        return const_objects_ + size_;
    }
    const T &operator[](std::size_t i) const {
        HSDS_DEBUG_IF(i >= size_, HSDS_BOUND_ERROR);
        return const_objects_[i];
    }
    const T &front() const {
        HSDS_DEBUG_IF(size_ == 0, HSDS_STATE_ERROR);
        return const_objects_[0];
    }
    const T &back() const {
        HSDS_DEBUG_IF(size_ == 0, HSDS_STATE_ERROR);
        return const_objects_[size_ - 1];
    }

    T *begin() {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);
        return objects_;
    }
    T *end() {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);
        return objects_ + size_;
    }
    T &operator[](std::size_t i) {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);
        HSDS_DEBUG_IF(i >= size_, HSDS_BOUND_ERROR);
        return objects_[i];
    }
    T &front() {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);
        HSDS_DEBUG_IF(size_ == 0, HSDS_STATE_ERROR);
        return objects_[0];
    }
    T &back() {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);
        HSDS_DEBUG_IF(size_ == 0, HSDS_STATE_ERROR);
        return objects_[size_ - 1];
    }

    std::size_t size() const {
        return size_;
    }
    std::size_t capacity() const {
        return capacity_;
    }
    bool fixed() const {
        return fixed_;
    }

    bool empty() const {
        return size_ == 0;
    }
    std::size_t total_size() const {
        return sizeof(T) * size_;
    }
    std::size_t io_size() const {
        return sizeof(UInt64) + ((total_size() + 7) & ~(std::size_t) 0x07);
    }

    void clear() {
        Vector().swap(*this);
    }
    void swap(Vector &rhs) {
        buf_.swap(rhs.buf_);
        HSDS::swap(objects_, rhs.objects_);
        HSDS::swap(const_objects_, rhs.const_objects_);
        HSDS::swap(size_, rhs.size_);
        HSDS::swap(capacity_, rhs.capacity_);
        HSDS::swap(fixed_, rhs.fixed_);
    }

    static std::size_t max_size() {
        return HSDS_SIZE_MAX / sizeof(T);
    }

private:
    ScopedArray<char> buf_;
    T *objects_;
    const T *const_objects_;
    std::size_t size_;
    std::size_t capacity_;
    bool fixed_;

    void map_(std::istream& is) {
        UInt64 total_size;
        mapper.map(&total_size);
        HSDS_EXCEPTION_IF(total_size > HSDS_SIZE_MAX, HSDS_SIZE_ERROR);
        HSDS_EXCEPTION_IF((total_size % sizeof(T)) != 0, HSDS_FORMAT_ERROR);
        const std::size_t size = (std::size_t)(total_size / sizeof(T));
        mapper.map(&const_objects_, size);
        mapper.seek((std::size_t)((8 - (total_size % 8)) % 8));
        size_ = size;
        fix();
    }

    void read_(std::istream& is) {
        size_t total_size;
        is.read((char *)&total_size, sizeof(size_t));
        HSDS_EXCEPTION_IF(total_size > HSDS_SIZE_MAX, HSDS_SIZE_ERROR);
        HSDS_EXCEPTION_IF((total_size % sizeof(T)) != 0, HSDS_FORMAT_ERROR);
        const std::size_t size = (std::size_t)(total_size / sizeof(T));
        resize(size);
        is.read((char *)objects_, total_size);
    }

    void write_(std::ostream& os) const {
        size_t totalSize = total_size();
        os.write((char*)&totalSize, sizeof(size_t));
        os.write((char*)const_objects_, size_);
    }

    // realloc() assumes that T's placement new does not throw an exception.
    void realloc(std::size_t new_capacity) {
        HSDS_DEBUG_IF(new_capacity > max_size(), HSDS_SIZE_ERROR);

        ScopedArray<char> new_buf(new (std::nothrow) char[sizeof(T) * new_capacity]);
        HSDS_DEBUG_IF(new_buf.get() == NULL, HSDS_MEMORY_ERROR);
        T *new_objects = reinterpret_cast<T *>(new_buf.get());

        for (std::size_t i = 0; i < size_; ++i) {
            new (&new_objects[i]) T(objects_[i]);
        }
        for (std::size_t i = 0; i < size_; ++i) {
            objects_[i].~T();
        }

        buf_.swap(new_buf);
        objects_ = new_objects;
        const_objects_ = new_objects;
        capacity_ = new_capacity;
    }

    // Disallows copy and assignment.
    Vector(const Vector &);
    Vector &operator=(const Vector &);
};

}

#endif /* !defined(HSDS_VECTOR_HPP_) */
