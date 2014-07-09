/**
 * @file scoped_ptr.hpp
 * @brief Implementation of scoped pointer
 * @author Hideaki Ohno
 */

#if !defined(HSDS_SCOPED_PTR_HPP_)
#define HSDS_SCOPED_PTR_HPP_

#include <algorithm>
#include "hsds/exception.hpp"
#include "hsds/constants.hpp"

namespace hsds {

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
        return *ptr_;
    }
    T *operator->() const {
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

    // Disable copy constructor
    ScopedPtr(const ScopedPtr &);

    // Disable assingment operator
    ScopedPtr &operator=(const ScopedPtr &);
};

} // namespace hsds

#endif /* !defined(HSDS_SCOPED_PTR_HPP_) */
