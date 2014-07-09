/**
 * @file vector.hpp
 * @author Hideaki Ohno
 */

#if !defined(HSDS_VECTOR_HPP_)
#define HSDS_VECTOR_HPP_

#include <cstddef>
#include "hsds/scoped_array.hpp"
#include "hsds/constants.hpp"
#include "hsds/exception.hpp"

namespace hsds {

template<typename T>
class Vector {
public:
    Vector() :
            buf_(), objects_(NULL), const_objects_(NULL), size_(0), capacity_(0), fixed_(false) {
    }

    Vector(const Vector& rhs) {
        size_ = rhs.size_;
        capacity_ = rhs.capacity_;
        fixed_ = rhs.fixed_;
        ScopedArray<char> new_buf(new (std::nothrow) char[sizeof(T) * capacity_]);
        buf_.swap(new_buf);
        objects_ = reinterpret_cast<T *>(buf_.get());
        const_objects_ = objects_;
        for (uint64_t i = 0; i < size_; ++i) {
            new (&objects_[i]) T(rhs.objects_[i]);
        }
    }

    ~Vector() {
        for (uint64_t i = 0; i < size_; ++i) {
            objects_[i].~T();
        }
    }

    uint64_t map(void *ptr, uint64_t size) {
        Vector temp;
        uint64_t offset = temp.map_(ptr, size);
        swap(temp);
        return offset;
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
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);HSDS_DEBUG_IF(size_ == max_size(), HSDS_SIZE_ERROR);
        reserve(size_ + 1);
        new (&objects_[size_]) T(x);
        ++size_;
    }

    void pop_back() {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);HSDS_DEBUG_IF(size_ == 0, HSDS_STATE_ERROR);
        objects_[--size_].~T();
    }

    // resize() assumes that T's placement new does not throw an exception.
    void resize(uint64_t size) {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);
        reserve(size);
        for (uint64_t i = size_; i < size; ++i) {
            new (&objects_[i]) T;
        }
        for (uint64_t i = size; i < size_; ++i) {
            objects_[i].~T();
        }
        size_ = size;
    }

    // resize() assumes that T's placement new does not throw an exception.
    void resize(uint64_t size, const T &x) {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);
        reserve(size);
        for (uint64_t i = size_; i < size; ++i) {
            new (&objects_[i]) T(x);
        }
        for (uint64_t i = size; i < size_; ++i) {
            objects_[i].~T();
        }
        size_ = size;
    }

    void reserve(uint64_t capacity) {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);
        if (capacity <= capacity_) {
            return;
        }HSDS_DEBUG_IF(capacity > max_size(), HSDS_SIZE_ERROR);
        uint64_t new_capacity = capacity;
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
        HSDS_EXCEPTION_IF(fixed_, HSDS_STATE_ERROR);
        if (size_ != capacity_) {
            realloc(size_);
        }
    }

    void fix() {
        HSDS_EXCEPTION_IF(fixed_, HSDS_STATE_ERROR);
        fixed_ = true;
    }

    const T *begin() const {
        return const_objects_;
    }
    const T *end() const {
        return const_objects_ + size_;
    }
    const T &operator[](uint64_t i) const {
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
    T &operator[](uint64_t i) {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);HSDS_DEBUG_IF(i >= size_, HSDS_BOUND_ERROR);
        return objects_[i];
    }
    T &front() {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);HSDS_DEBUG_IF(size_ == 0, HSDS_STATE_ERROR);
        return objects_[0];
    }
    T &back() {
        HSDS_DEBUG_IF(fixed_, HSDS_STATE_ERROR);HSDS_DEBUG_IF(size_ == 0, HSDS_STATE_ERROR);
        return objects_[size_ - 1];
    }

    uint64_t size() const {
        return size_;
    }
    uint64_t capacity() const {
        return capacity_;
    }
    bool fixed() const {
        return fixed_;
    }

    bool empty() const {
        return size_ == 0;
    }
    uint64_t total_size() const {
        return sizeof(T) * size_;
    }
    uint64_t io_size() const {
        return sizeof(uint64_t) + ((total_size() + 7) & ~(uint64_t) 0x07);
    }

    void clear() {
        Vector().swap(*this);
    }
    void swap(Vector &rhs) {
        buf_.swap(rhs.buf_);
        std::swap(objects_, rhs.objects_);
        std::swap(const_objects_, rhs.const_objects_);
        std::swap(size_, rhs.size_);
        std::swap(capacity_, rhs.capacity_);
        std::swap(fixed_, rhs.fixed_);
    }

    static uint64_t max_size() {
        return HSDS_SIZE_MAX / sizeof(T);
    }

private:
    ScopedArray<char> buf_;
    T *objects_;
    const T *const_objects_;
    uint64_t size_;
    uint64_t capacity_;
    bool fixed_;

    uint64_t map_(void* ptr, uint64_t mapSize) {
        uint64_t offset = 0;
        uint64_t data_num = *reinterpret_cast<uint64_t*>(ptr);
        offset += sizeof(data_num);
        uint64_t total_size = data_num * sizeof(T);
        HSDS_EXCEPTION_IF(total_size > HSDS_SIZE_MAX, HSDS_SIZE_ERROR);
        HSDS_EXCEPTION_IF(total_size > mapSize, HSDS_SIZE_ERROR);
        const_objects_ = reinterpret_cast<T*>(static_cast<char*>(ptr) + offset);
        offset += total_size;
        size_ = data_num;
        fix();
        return offset;
    }

    void read_(std::istream& is) {
        uint64_t data_num;
        is.read(reinterpret_cast<char*>(&data_num), sizeof(uint64_t));
        HSDS_EXCEPTION_IF((data_num * sizeof(T)) > HSDS_SIZE_MAX, HSDS_SIZE_ERROR);
        resize(data_num);
        is.read(reinterpret_cast<char*>(objects_), data_num * sizeof(T));
    }

    void write_(std::ostream& os) const {
        uint64_t data_num = size();
        os.write(reinterpret_cast<const char*>(&data_num), sizeof(uint64_t));
        os.write(reinterpret_cast<const char*>(const_objects_), total_size());
    }

    // realloc() assumes that T's placement new does not throw an exception.
    void realloc(uint64_t new_capacity) {
        HSDS_DEBUG_IF(new_capacity > max_size(), HSDS_SIZE_ERROR);
        ScopedArray<char> new_buf(new (std::nothrow) char[sizeof(T) * new_capacity]);
        HSDS_DEBUG_IF(new_buf.get() == NULL, HSDS_MEMORY_ERROR);
        T *new_objects = reinterpret_cast<T *>(new_buf.get());

        for (uint64_t i = 0; i < size_; ++i) {
            new (&new_objects[i]) T(objects_[i]);
        }
        for (uint64_t i = 0; i < size_; ++i) {
            objects_[i].~T();
        }

        buf_.swap(new_buf);
        objects_ = new_objects;
        const_objects_ = new_objects;
        capacity_ = new_capacity;
    }

    // Disable assingment operator
    Vector &operator=(const Vector &);
};

} // namespace hsds

#endif /* !defined(HSDS_VECTOR_HPP_) */

