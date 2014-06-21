/**
 * @file scoped_array.hpp
 * @brief Implementation of scoped array
 * @author Hidaki Ohno
 */

#if !defined(HSDS_SCOPED_ARRAY_HPP_)
#define HSDS_SCOPED_ARRAY_HPP_

#include <algorithm>
#include "hsds/exception.hpp"
#include "hsds/constants.hpp"

namespace hsds {

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
        HSDS_EXCEPTION_IF((array != NULL) && (array == array_), HSDS_RESET_ERROR);
        ScopedArray(array).swap(*this);
    }

    T &operator[](std::size_t i) const {
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

    // Disable assingment operator
    ScopedArray &operator=(const ScopedArray &);
};

} // namespace hsds

#endif /* !defined(HSDS_SCOPED_ARRAY_HPP_) */
