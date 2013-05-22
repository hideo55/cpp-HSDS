#ifndef HSDS_POPCOUNT_H_
#define HSDS_POPCOUNT_H_

#include <stdint.h>

#ifdef HSDS_USE_SSE3
#include <xmmintrin.h>
#include <tmmintrin.h>
#endif

#ifdef HSDS_USE_POPCNT
 #ifdef _MSC_VER
  #include <intrin.h>
 #else  // _MSC_VERa
  #include <popcntintrin.h>
 #endif  // _MSC_VER
#endif

namespace hsds {
class PopCount {
public:
    PopCount(uint64_t x) {
        x = (x & 0x5555555555555555ULL) + ((x & 0xAAAAAAAAAAAAAAAAULL) >> 1);
        x = (x & 0x3333333333333333ULL) + ((x & 0xCCCCCCCCCCCCCCCCULL) >> 2);
        x = (x & 0x0F0F0F0F0F0F0F0FULL) + ((x & 0xF0F0F0F0F0F0F0F0ULL) >> 4);
        x *= 0x0101010101010101ULL;
        value_ = x;
    }
    ~PopCount() {
    }

    uint64_t lo8() const {
        return (uint64_t) (value_ & 0xFFU);
    }

    uint64_t lo16() const {
        return (uint64_t) ((value_ >> 8) & 0xFFU);
    }

    uint64_t lo24() const {
        return (uint64_t) ((value_ >> 16) & 0xFFU);
    }

    uint64_t lo32() const {
        return (uint64_t) ((value_ >> 24) & 0xFFU);
    }

    uint64_t lo40() const {
        return (uint64_t) ((value_ >> 32) & 0xFFU);
    }

    uint64_t lo48() const {
        return (uint64_t) ((value_ >> 40) & 0xFFU);
    }

    uint64_t lo56() const {
        return (uint64_t) ((value_ >> 48) & 0xFFU);
    }

    uint64_t lo64() const {
        return (uint64_t) ((value_ >> 56) & 0xFFU);;
    }

    static uint64_t count(uint64_t x) {
#if defined(HSDS_USE_POPCNT)
#ifdef _MSC_VER
        return __popcnt64(x);
#else
        return _mm_popcnt_u64(x);
#endif
#else
        return PopCount(x).lo64();
#endif
    }

private:
    uint64_t value_;
};
}

#endif /* HSDS_POPCOUNT_H_ */
