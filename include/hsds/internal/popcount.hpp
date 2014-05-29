#if !defined(HSDS_POPCOUNT_H_)
#define HSDS_POPCOUNT_H_

#if !defined(_MSC_VER)
#include <stdint.h>
#endif // !defined(_MSC_VER)
#include "hsds/internal/intrin.h"

namespace hsds {

/**
 * @class PopCount
 *
 * @brief Calculate hamming weight of 64-bit integer
 */
class PopCount {
public:
    /**
     * @brief Constructor
     *
     * @param[in] x 64-bit integer
     */
    PopCount(uint64_t x) {
        x = (x & 0x5555555555555555ULL) + ((x & 0xAAAAAAAAAAAAAAAAULL) >> 1);
        x = (x & 0x3333333333333333ULL) + ((x & 0xCCCCCCCCCCCCCCCCULL) >> 2);
        x = (x & 0x0F0F0F0F0F0F0F0FULL) + ((x & 0xF0F0F0F0F0F0F0F0ULL) >> 4);
        x *= 0x0101010101010101ULL;
        value_ = x;
    }

    /**
     * @brief Destructor
     */
    ~PopCount() {
    }

    /**
     * @brief Returns hamming weight of lowest 8-bit
     *
     * @return Hamming weight of lowest 8-bit
     */
    uint64_t lo8() const {
        return (uint64_t) (value_ & 0xFFU);
    }

    /**
     * @brief Returns hamming weight of lowest 16-bit
     *
     * @return Hamming weight of lowest 16-bit
     */
    uint64_t lo16() const {
        return (uint64_t) ((value_ >> 8) & 0xFFU);
    }

    /**
     * @brief Returns hamming weight of lowest 24-bit
     *
     * @return Hamming weight of lowest 24-bit
     */
    uint64_t lo24() const {
        return (uint64_t) ((value_ >> 16) & 0xFFU);
    }

    /**
     * @brief Returns hamming weight of lowest 32-bit
     *
     * @return Hamming weight of lowest 32-bit
     */
    uint64_t lo32() const {
        return (uint64_t) ((value_ >> 24) & 0xFFU);
    }

    /**
     * @brief Returns hamming weight of lowest 40-bit
     *
     * @return Hamming weight of lowest 40-bit
     */
    uint64_t lo40() const {
        return (uint64_t) ((value_ >> 32) & 0xFFU);
    }

    /**
     * @brief Returns hamming weight of lowest 48-bit
     *
     * @return Hamming weight of lowest 48-bit
     */
    uint64_t lo48() const {
        return (uint64_t) ((value_ >> 40) & 0xFFU);
    }

    /**
     * @brief Returns hamming weight of lowest 56-bit
     *
     * @return Hamming weight of lowest 56-bit
     */
    uint64_t lo56() const {
        return (uint64_t) ((value_ >> 48) & 0xFFU);
    }

    /**
     * @brief Returns hamming weight of lowest 64-bit
     *
     * @return Hamming weight of lowest 64-bit
     */
    uint64_t lo64() const {
        return (uint64_t) ((value_ >> 56) & 0xFFU);
    }

    /**
     * @brief Calculate hamming weight of 64-bit integer
     *
     * @param[in] x 64-bit integer
     */
    static uint64_t count(uint64_t x) {
#if defined(HSDS_USE_POPCNT)
 #if defined(_MSC_VER)
        return __popcnt64(x);
 #else // defined(_MSC_VER)
        return _mm_popcnt_u64(x);
 #endif // defined(_MSC_VER)
#else // defined(HSDS_USE_POPCNT)
        return PopCount(x).lo64();
#endif // defined(HSDS_USE_POPCNT)
    }

private:
    uint64_t value_;
};
}

#endif /* !defined(HSDS_POPCOUNT_H_) */
