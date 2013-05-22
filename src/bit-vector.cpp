#include "hsds/bit-vector.hpp"

#if defined(_MSC_VER)

#define FORCE_INLINE    __forceinline

#else

#define FORCE_INLINE    inline __attribute__((always_inline))

#endif

namespace hsds {
using namespace std;

const uint8_t SELECT_TABLE[8][256] =
{
{ 7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1,
        0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0,
        2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0,
        1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1,
        0, 2, 0, 1, 0, 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0 },
{ 7, 7, 7, 1, 7, 2, 2, 1, 7, 3, 3, 1, 3, 2, 2, 1, 7, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1, 7, 5, 5, 1, 5, 2, 2,
        1, 5, 3, 3, 1, 3, 2, 2, 1, 5, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1, 7, 6, 6, 1, 6, 2, 2, 1, 6, 3, 3, 1,
        3, 2, 2, 1, 6, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1, 6, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1, 3, 2, 2, 1, 5,
        4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1, 7, 7, 7, 1, 7, 2, 2, 1, 7, 3, 3, 1, 3, 2, 2, 1, 7, 4, 4, 1, 4, 2,
        2, 1, 4, 3, 3, 1, 3, 2, 2, 1, 7, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1, 3, 2, 2, 1, 5, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3,
        1, 3, 2, 2, 1, 7, 6, 6, 1, 6, 2, 2, 1, 6, 3, 3, 1, 3, 2, 2, 1, 6, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1,
        6, 5, 5, 1, 5, 2, 2, 1, 5, 3, 3, 1, 3, 2, 2, 1, 5, 4, 4, 1, 4, 2, 2, 1, 4, 3, 3, 1, 3, 2, 2, 1 },
{ 7, 7, 7, 7, 7, 7, 7, 2, 7, 7, 7, 3, 7, 3, 3, 2, 7, 7, 7, 4, 7, 4, 4, 2, 7, 4, 4, 3, 4, 3, 3, 2, 7, 7, 7, 5, 7, 5, 5,
        2, 7, 5, 5, 3, 5, 3, 3, 2, 7, 5, 5, 4, 5, 4, 4, 2, 5, 4, 4, 3, 4, 3, 3, 2, 7, 7, 7, 6, 7, 6, 6, 2, 7, 6, 6, 3,
        6, 3, 3, 2, 7, 6, 6, 4, 6, 4, 4, 2, 6, 4, 4, 3, 4, 3, 3, 2, 7, 6, 6, 5, 6, 5, 5, 2, 6, 5, 5, 3, 5, 3, 3, 2, 6,
        5, 5, 4, 5, 4, 4, 2, 5, 4, 4, 3, 4, 3, 3, 2, 7, 7, 7, 7, 7, 7, 7, 2, 7, 7, 7, 3, 7, 3, 3, 2, 7, 7, 7, 4, 7, 4,
        4, 2, 7, 4, 4, 3, 4, 3, 3, 2, 7, 7, 7, 5, 7, 5, 5, 2, 7, 5, 5, 3, 5, 3, 3, 2, 7, 5, 5, 4, 5, 4, 4, 2, 5, 4, 4,
        3, 4, 3, 3, 2, 7, 7, 7, 6, 7, 6, 6, 2, 7, 6, 6, 3, 6, 3, 3, 2, 7, 6, 6, 4, 6, 4, 4, 2, 6, 4, 4, 3, 4, 3, 3, 2,
        7, 6, 6, 5, 6, 5, 5, 2, 6, 5, 5, 3, 5, 3, 3, 2, 6, 5, 5, 4, 5, 4, 4, 2, 5, 4, 4, 3, 4, 3, 3, 2 },
{ 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 7, 7, 7, 7, 7, 7, 7, 4, 7, 7, 7, 4, 7, 4, 4, 3, 7, 7, 7, 7, 7, 7, 7,
        5, 7, 7, 7, 5, 7, 5, 5, 3, 7, 7, 7, 5, 7, 5, 5, 4, 7, 5, 5, 4, 5, 4, 4, 3, 7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 7, 6,
        7, 6, 6, 3, 7, 7, 7, 6, 7, 6, 6, 4, 7, 6, 6, 4, 6, 4, 4, 3, 7, 7, 7, 6, 7, 6, 6, 5, 7, 6, 6, 5, 6, 5, 5, 3, 7,
        6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 3, 7, 7, 7, 7, 7, 7,
        7, 4, 7, 7, 7, 4, 7, 4, 4, 3, 7, 7, 7, 7, 7, 7, 7, 5, 7, 7, 7, 5, 7, 5, 5, 3, 7, 7, 7, 5, 7, 5, 5, 4, 7, 5, 5,
        4, 5, 4, 4, 3, 7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 7, 6, 7, 6, 6, 3, 7, 7, 7, 6, 7, 6, 6, 4, 7, 6, 6, 4, 6, 4, 4, 3,
        7, 7, 7, 6, 7, 6, 6, 5, 7, 6, 6, 5, 6, 5, 5, 3, 7, 6, 6, 5, 6, 5, 5, 4, 6, 5, 5, 4, 5, 4, 4, 3 },
{ 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 4, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 5, 7, 7, 7, 7, 7, 7, 7, 5, 7, 7, 7, 5, 7, 5, 5, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 6, 7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 7, 6, 7, 6, 6, 4, 7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 7, 6, 7, 6, 6, 5, 7,
        7, 7, 6, 7, 6, 6, 5, 7, 6, 6, 5, 6, 5, 5, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 5, 7, 7, 7, 7, 7, 7, 7, 5, 7, 7, 7,
        5, 7, 5, 5, 4, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 7, 6, 7, 6, 6, 4,
        7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 7, 6, 7, 6, 6, 5, 7, 7, 7, 6, 7, 6, 6, 5, 7, 6, 6, 5, 6, 5, 5, 4 },
{ 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 7,
        7, 7, 7, 7, 7, 7, 6, 7, 7, 7, 6, 7, 6, 6, 5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 5, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 7, 6, 7, 6, 6, 5 },
{ 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6 },
{ 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
        7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7 } };

const uint64_t MASK_55 = 0x5555555555555555ULL;
const uint64_t MASK_33 = 0x3333333333333333ULL;
const uint64_t MASK_0F = 0x0F0F0F0F0F0F0F0FULL;
const uint64_t MASK_01 = 0x0101010101010101ULL;
const uint64_t MASK_80 = 0x8080808080808080ULL;

FORCE_INLINE uint64_t select_bit(uint64_t i, uint64_t bit_id, uint64_t block) {
    uint64_t counts;
    {
#if defined(HSDS_USE_SSE3)
        __m128i lower_nibbles = _mm_cvtsi64_si128(block & 0x0F0F0F0F0F0F0F0FULL);
        __m128i upper_nibbles = _mm_cvtsi64_si128(block & 0xF0F0F0F0F0F0F0F0ULL);
        upper_nibbles = _mm_srli_epi32(upper_nibbles, 4);

        __m128i lower_counts =
        _mm_set_epi8(4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0);
        lower_counts = _mm_shuffle_epi8(lower_counts, lower_nibbles);
        __m128i upper_counts =
        _mm_set_epi8(4, 3, 3, 2, 3, 2, 2, 1, 3, 2, 2, 1, 2, 1, 1, 0);
        upper_counts = _mm_shuffle_epi8(upper_counts, upper_nibbles);

        counts = _mm_cvtsi128_si64(_mm_add_epi8(lower_counts, upper_counts));
#else // defined(HSDS_USE_SSE3)
        counts = block - ((block >> 1) & MASK_55);
        counts = (counts & MASK_33) + ((counts >> 2) & MASK_33);
        counts = (counts + (counts >> 4)) & MASK_0F;
#endif // defined(HSDS_USE_SSE3)
        counts *= MASK_01;
    }
#if defined(HSDS_USE_POPCNT)
    uint8_t skip;
    {
        __m128i x = _mm_cvtsi64_si128((i + 1) * MASK_01);
        __m128i y = _mm_cvtsi64_si128(counts);
        x = _mm_cmpgt_epi8(x, y);
        skip = (uint8_t)PopCount::count(_mm_cvtsi128_si64(x));
    }
#else // defined(HSDS_USE_POPCNT)
    const uint64_t x = (counts | MASK_80) - ((i + 1) * MASK_01);
#ifdef _MSC_VER
    unsigned long skip;
    ::_BitScanForward64(&skip, (x & MASK_80) >> 7);
    --skip;
#else // _MSC_VER
    const int skip = ::__builtin_ctzll((x & MASK_80) >> 7);
#endif // _MSC_VER
#endif // defined(HSDS_USE_POPCNT)
    bit_id += skip;
    block >>= skip;
    i -= ((counts << 8) >> skip) & 0xFF;
    return bit_id + SELECT_TABLE[i][block & 0xFF];
}

FORCE_INLINE uint64_t rank64(uint64_t x, uint64_t i, bool b) {
    if (!b) {
        x = ~x;
    }
    x <<= (S_BLOCK_SIZE - i);
    return PopCount::count(x);
}

FORCE_INLINE uint64_t select64(uint64_t x, uint64_t i, bool b) {
    if (!b) {
        x = ~x;
    }
    uint64_t x1 = ((x & 0xaaaaaaaaaaaaaaaaULL) >> 1) + (x & 0x5555555555555555ULL);
    uint64_t x2 = ((x1 & 0xccccccccccccccccULL) >> 2) + (x1 & 0x3333333333333333ULL);
    uint64_t x3 = ((x2 & 0xf0f0f0f0f0f0f0f0ULL) >> 4) + (x2 & 0x0f0f0f0f0f0f0f0fULL);
    uint64_t x4 = ((x3 & 0xff00ff00ff00ff00ULL) >> 8) + (x3 & 0x00ff00ff00ff00ffULL);
    uint64_t x5 = ((x4 & 0xffff0000ffff0000ULL) >> 16) + (x4 & 0x0000ffff0000ffffULL);

    i++;
    uint64_t pos = 0;
    uint64_t v5 = x5 & 0xffffffffULL;
    if (i > v5) {
        i -= v5;
        pos += 32;
    }
    uint64_t v4 = (x4 >> pos) & 0x0000ffffULL;
    if (i > v4) {
        i -= v4;
        pos += 16;
    }
    uint64_t v3 = (x3 >> pos) & 0x000000ffULL;
    if (i > v3) {
        i -= v3;
        pos += 8;
    }
    uint64_t v2 = (x2 >> pos) & 0x0000000fULL;
    if (i > v2) {
        i -= v2;
        pos += 4;
    }
    uint64_t v1 = (x1 >> pos) & 0x00000003ULL;
    if (i > v1) {
        i -= v1;
        pos += 2;
    }
    uint64_t v0 = (x >> pos) & 0x00000001ULL;
    if (i > v0) {
        i -= v0;
        pos += 1;
    }
    return pos;
}

BitVector::BitVector() :
        size_(0), num_of_1s_(0) {
}

BitVector::~BitVector() {
}

BitVector::BitVector(uint64_t size) :
        num_of_1s_(0) {
    size_ = size;
    uint64_t block_num = (size + S_BLOCK_SIZE - 1) / S_BLOCK_SIZE;
    blocks_.resize(block_num);
}

void BitVector::clear() {
    blocks_.clear();
    blocks_type().swap(blocks_);
    rank_tables_.clear();
    ranktable_type().swap(rank_tables_);
    size_ = 0;
    num_of_1s_ = 0;
}

bool BitVector::get(uint64_t i) const {
    if (i > size_) {
        throw "Out of range access in hsds::BitVector::get()";
    }
    return (blocks_[i / S_BLOCK_SIZE] & (1ULL << (i % S_BLOCK_SIZE))) != 0;
}

void BitVector::set(uint64_t i, bool b) {
    if (i >= size_) {
        size_ = i + 1;
    }
    uint64_t q = i / S_BLOCK_SIZE;
    uint64_t r = i % S_BLOCK_SIZE;

    while (q >= blocks_.size()) {
        blocks_.push_back(0);
    }
    uint64_t m = 0x1ULL << r;
    if (b) {
        blocks_[q] |= m;
    } else {
        blocks_[q] &= ~m;
    }
}

void BitVector::build() {
    rank_tables_.clear();
    uint64_t block_num = blocks_.size();
    uint64_t num_0s_in_lblock = 0, num_1s_in_lblock = 0;
    num_of_1s_ = 0;

    rank_tables_.clear();
    ranktable_type().swap(rank_tables_);
    rank_tables_.resize(
            ((block_num * S_BLOCK_SIZE) / L_BLOCK_SIZE) + (((block_num * S_BLOCK_SIZE) % L_BLOCK_SIZE) != 0 ? 1 : 0)
                    + 1);
    select_0s_.clear();
    select_dict_type().swap(select_0s_);
    select_0s_.push_back(0);

    select_1s_.clear();
    select_dict_type().swap(select_1s_);
    select_1s_.push_back(0);

    for (uint64_t i = 0; i < block_num; ++i) {
        uint64_t rank_id = i / BLOCK_RATE;
        RankIndex &rank = rank_tables_[rank_id];
        switch (i % 8) {
        case 0: {
            rank.set_abs(num_of_1s_);
            break;
        }
        case 1: {
            rank.set_rel1(num_of_1s_ - rank.abs());
            break;
        }
        case 2: {
            rank.set_rel2(num_of_1s_ - rank.abs());
            break;
        }
        case 3: {
            rank.set_rel3(num_of_1s_ - rank.abs());
            break;
        }
        case 4: {
            rank.set_rel4(num_of_1s_ - rank.abs());
            break;
        }
        case 5: {
            rank.set_rel5(num_of_1s_ - rank.abs());
            break;
        }
        case 6: {
            rank.set_rel6(num_of_1s_ - rank.abs());
            break;
        }
        case 7: {
            rank.set_rel7(num_of_1s_ - rank.abs());
            break;
        }
        }
        uint64_t count1s = PopCount::count(blocks_[i]);

        if (num_1s_in_lblock + count1s > L_BLOCK_SIZE) {
            uint32_t diff = L_BLOCK_SIZE - num_1s_in_lblock;
            uint32_t pos = select64(blocks_[i], count1s - diff, true);
            select_1s_.push_back((i - 1) * S_BLOCK_SIZE + pos);
            num_1s_in_lblock -= L_BLOCK_SIZE;
        }
        uint64_t count0s = S_BLOCK_SIZE - count1s;
        if (num_0s_in_lblock + count0s > L_BLOCK_SIZE) {
            uint32_t diff = L_BLOCK_SIZE - num_0s_in_lblock;
            uint32_t pos = select64(blocks_[i], count0s - diff, false);
            select_0s_.push_back((i - 1) * S_BLOCK_SIZE + pos);
            num_0s_in_lblock -= L_BLOCK_SIZE;
        }
        num_1s_in_lblock += count1s;
        num_0s_in_lblock += count0s;
        num_of_1s_ += count1s;
    }

    if ((block_num % BLOCK_RATE) != 0) {
        uint64_t rank_id = (block_num - 1) / BLOCK_RATE;
        RankIndex &rank = rank_tables_[rank_id];
        switch ((block_num - 1) % BLOCK_RATE) {
        case 0: {
            rank.set_rel1(num_of_1s_ - rank.abs());
        }
        case 1: {
            rank.set_rel2(num_of_1s_ - rank.abs());
        }
        case 2: {
            rank.set_rel3(num_of_1s_ - rank.abs());
        }
        case 3: {
            rank.set_rel4(num_of_1s_ - rank.abs());
        }
        case 4: {
            rank.set_rel5(num_of_1s_ - rank.abs());
        }
        case 5: {
            rank.set_rel6(num_of_1s_ - rank.abs());
        }
        case 6: {
            rank.set_rel7(num_of_1s_ - rank.abs());
        }
        }

    }
    rank_tables_.back().set_abs(num_of_1s_);
    select_0s_.push_back(size_);
    select_1s_.push_back(size_);
}

uint64_t BitVector::rank(uint64_t i) const {
    if (i > this->size()) {
        throw "Out of range access in hsds::BitVector::rank()";
    }
    uint64_t q_large = i / L_BLOCK_SIZE;
    uint64_t q_small = i / S_BLOCK_SIZE;
    uint64_t r = i % S_BLOCK_SIZE;

    const RankIndex &rank = rank_tables_[q_large];
    uint64_t offset = rank.abs();
    switch (q_small % BLOCK_RATE) {
    case 1:
        offset += rank.rel1();
        break;
    case 2:
        offset += rank.rel2();
        break;
    case 3:
        offset += rank.rel3();
        break;
    case 4:
        offset += rank.rel4();
        break;
    case 5:
        offset += rank.rel5();
        break;
    case 6:
        offset += rank.rel6();
        break;
    case 7:
        offset += rank.rel7();
        break;
    }
    offset += PopCount::count(blocks_[q_small] & ((1ULL << r) - 1));
    return offset;
}
uint64_t BitVector::select0(uint64_t i) const {
    if (i >= this->size(false)) {
        throw "Out of range access in hsds::BitVector::select()";
    }

    const uint64_t select_id = i / L_BLOCK_SIZE;
    if ((i % L_BLOCK_SIZE) == 0) {
        return select_0s_[select_id];
    }
    uint64_t begin = select_0s_[select_id] / L_BLOCK_SIZE;
    uint64_t end = (select_0s_[select_id + 1] + L_BLOCK_SIZE - 1) / L_BLOCK_SIZE;
    if (begin + 10 >= end) {
        while (i >= ((begin + 1) * L_BLOCK_SIZE) - rank_tables_[begin + 1].abs()) {
            ++begin;
        }
    } else {
        while (begin + 1 < end) {
            const uint64_t pivot = (begin + end) / 2;
            if (i < (pivot * 512) - rank_tables_[pivot].abs()) {
                end = pivot;
            } else {
                begin = pivot;
            }
        }
    }

    uint64_t rank_id = begin;
    i -= (rank_id * L_BLOCK_SIZE) - rank_tables_[rank_id].abs();

    const RankIndex &rank = rank_tables_[rank_id];
    uint64_t block_id = rank_id * BLOCK_RATE;
    if (i < (256U - rank.rel4())) {
        if (i < (128U - rank.rel2())) {
            if (i >= (64U - rank.rel1())) {
                block_id += 1;
                i -= 64 - rank.rel1();
            }
        } else if (i < (192U - rank.rel3())) {
            block_id += 2;
            i -= 128 - rank.rel2();
        } else {
            block_id += 3;
            i -= 192 - rank.rel3();
        }
    } else if (i < (384U - rank.rel6())) {
        if (i < (320U - rank.rel5())) {
            block_id += 4;
            i -= 256 - rank.rel4();
        } else {
            block_id += 5;
            i -= 320 - rank.rel5();
        }
    } else if (i < (448U - rank.rel7())) {
        block_id += 6;
        i -= 384 - rank.rel6();
    } else {
        block_id += 7;
        i -= 448 - rank.rel7();
    }
    return select_bit(i, block_id * S_BLOCK_SIZE, ~blocks_[block_id]);
}

uint64_t BitVector::select1(uint64_t i) const {
    if (i >= this->size(true)) {
        throw "Out of range access in hsds::BitVector::select()";
    }

    const uint64_t select_id = i / L_BLOCK_SIZE;
    if ((i % L_BLOCK_SIZE) == 0) {
        return select_1s_[select_id];
    }
    uint64_t begin = select_1s_[select_id] / L_BLOCK_SIZE;
    uint64_t end = (select_1s_[select_id + 1] + L_BLOCK_SIZE - 1) / L_BLOCK_SIZE;

    if (begin + 10 >= end) {
        // Linear search in rank table
        while (i >= rank_tables_[begin + 1].abs()) {
            ++begin;
        }
    } else {
        // Binary search in rank table
        while (begin + 1 < end) {
            uint64_t pivot = (begin + end) / 2;
            if (i < rank_tables_[pivot].abs()) {
                end = pivot;
            } else {
                begin = pivot;
            }
        }
    }

    uint64_t rank_id = begin;

    const RankIndex &rank = rank_tables_[rank_id];

    i -= rank.abs();
    uint64_t block_id = rank_id * BLOCK_RATE;

    if (i < rank.rel4()) {
        if (i < rank.rel2()) {
            if (i >= rank.rel1()) {
                block_id += 1;
                i -= rank.rel1();
            }
        } else if (i < rank.rel3()) {
            block_id += 2;
            i -= rank.rel2();
        } else {
            block_id += 3;
            i -= rank.rel3();
        }
    } else if (i < rank.rel6()) {
        if (i < rank.rel5()) {
            block_id += 4;
            i -= rank.rel4();
        } else {
            block_id += 5;
            i -= rank.rel5();
        }
    } else if (i < rank.rel7()) {
        block_id += 6;
        i -= rank.rel6();
    } else {
        block_id += 7;
        i -= rank.rel7();
    }

    return select_bit(i, block_id * S_BLOCK_SIZE, blocks_[block_id]);
}

void BitVector::save(std::ostream &os) const {
    os.write((char*) &size_, sizeof(size_));
    os.write((char*) &num_of_1s_, sizeof(num_of_1s_));

    uint64_t size = blocks_.size();
    os.write((char*) &size, sizeof(size));
    os.write((char*) &blocks_[0], sizeof(blocks_[0]) * size);

    size = rank_tables_.size();
    os.write((char*) &size, sizeof(size));
    os.write((char*) &rank_tables_[0], sizeof(rank_tables_[0]) * size);

    if (os.fail()) {
        throw "Failed to save the bit vector.";
    }
}

void BitVector::load(std::istream &is) {
    is.read((char*) &size_, sizeof(size_));
    if (is.eof()) {
        throw "Failed to read file. File format is invalid.";
    }

    is.read((char*) &num_of_1s_, sizeof(num_of_1s_));
    if (is.eof()) {
        throw "Failed to read file. File format is invalid.";
    }

    uint64_t size = 0;
    is.read((char*) &size, sizeof(size));
    if (is.eof()) {
        throw "Failed to read file. File format is invalid.";
    }

    blocks_.clear();
    blocks_type().swap(blocks_);
    blocks_.resize(size);
    is.read((char*) &blocks_[0], sizeof(blocks_[0]) * size);
    if (is.eof()) {
        throw "Failed to read file. File format is invalid.";
    }
    is.read((char*) &size, sizeof(size));
    if (is.eof()) {
        throw "Failed to read file. File format is invalid.";
    }

    rank_tables_.clear();
    ranktable_type().swap(rank_tables_);
    rank_tables_.resize(size);
    is.read((char*) &rank_tables_[0], sizeof(rank_tables_[0]) * size);
    if (is.fail()) {
        throw "Failed to read file. File format is invalid.";
    }

}

}
