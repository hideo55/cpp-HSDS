/*
 * scoped_ptr.hpp
 *
 *  Created on: 2014/05/23
 *      Author: Hideaki Ohno
 */

#if !defined(HSDS_SCOPED_PTR_HPP_)
#define HSDS_SCOPED_PTR_HPP_

#include <algorithm>
#include "exception.hpp"
#include "constants.hpp"

namespace hsds {

namespace {
const char* HSDS_ERROR_RESET = "Failed to reset";
const char* HSDS_ERROR_STATE = "Bad state";
}

template<typename T>
class ScopedPtr {
public:
    ScopedPtr() :
            ptr_(NULL) {
    }
    explicit ScopedPtr(T *ptr) :
            ptr_(ptr) {
    }

    ~ScopedPtr() {
        delete ptr_;
    }

    void reset(T *ptr = NULL) {
        HSDS_EXCEPTION_IF((ptr != NULL) && (ptr == ptr_), HSDS_RESET_ERROR);
        ScopedPtr(ptr).swap(*this);
    }

    T &operator*() const {
        HSDS_DEBUG_IF(ptr_ == NULL, HSDS_STATE_ERROR);
        return *ptr_;
    }
    T *operator->() const {
        HSDS_DEBUG_IF(ptr_ == NULL, HSDS_STATE_ERROR);
        return ptr_;
    }
    T *get() const {
        return ptr_;
    }

    void clear() {
        ScopedPtr().swap(*this);
    }
    void swap(ScopedPtr &rhs) {
        std::swap(ptr_, rhs.ptr_);
    }

private:
    T *ptr_;

    // Disallows copy and assignment.
    ScopedPtr(const ScopedPtr &);
    ScopedPtr &operator=(const ScopedPtr &);
};

} // namespace hsds

#endif /* !defined(HSDS_SCOPED_PTR_HPP_) */
