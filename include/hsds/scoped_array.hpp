/*
 * scoped_array.hpp
 *
 *  Created on: 2014/05/23
 *      Author: Hideaki Ohno
 */

#if !defined(HSDS_SCOPED_ARRAY_HPP_)
#define HSDS_SCOPED_ARRAY_HPP_

#include <algorithm>
#include "exception.hpp"

namespace hsds {

namespace {
const char* HSDS_ERROR_RESET = "Failed to reset";
const char* HSDS_ERROR_STATE = "Bad state";
}

template<typename T>
class ScopedArray {
public:
    ScopedArray() :
            array_(NULL) {
    }
    explicit ScopedArray(T *array) :
            array_(array) {
    }

    ~ScopedArray() {
        delete[] array_;
    }

    void reset(T *array = NULL) {
        HSDS_EXCEPTION_IF((array != NULL) && (array == array_), HSDS_ERROR_RESET);
        ScopedArray(array).swap(*this);
    }

    T &operator[](std::size_t i) const {
        HSDS_DEBUG_IF(array_ == NULL, HSDS_ERROR_STATE);
        return array_[i];
    }

    T *get() const {
        return array_;
    }

    void clear() {
        ScopedArray().swap(*this);
    }

    void swap(ScopedArray &rhs) {
        std::swap(array_, rhs.array_);
    }

private:
    T *array_;

    // Disallows copy and assignment.
    ScopedArray(const ScopedArray &);
    ScopedArray &operator=(const ScopedArray &);
};

} // namespace hsds

#endif /* !defined(HSDS_SCOPED_ARRAY_HPP_) */
