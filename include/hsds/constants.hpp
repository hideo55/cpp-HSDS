/**
 * @file constants.hpp
 * @brief Definition of constant variables
 * @author Hideaki Ohno
 */

#if !defined(HSDS_CONSTANTS_HPP_)
#define HSDS_CONSTANTS_HPP_

#include <cstddef>

namespace hsds {

// constants

const size_t HSDS_SIZE_MAX = ((size_t)~(size_t)0);


// Error messages
const char* const HSDS_RESET_ERROR = "Failed to reset";
const char* const HSDS_STATE_ERROR = "Bad state";
const char* const HSDS_SIZE_ERROR = "Invalid size";
const char* const HSDS_FORMAT_ERROR = "Invalid format";
const char* const HSDS_BOUND_ERROR = "Out of bound access";
const char* const HSDS_MEMORY_ERROR = "Bad alloc";
}


#endif /* !defined(HSDS_CONSTANTS_HPP_) */
