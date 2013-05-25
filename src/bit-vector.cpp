#include "hsds/bit-vector.hpp"

#if defined(_MSC_VER)

#define FORCE_INLINE    __forceinline

#else

#define FORCE_INLINE    inline __attribute__((always_inline))

#endif

namespace hsds {
using namespace std;

FORCE_INLINE uint64_t rank64(uint64_t x, uint64_t i, bool b) {
    if (!b) {
        x = ~x;
    }
    x <<= (S_BLOCK_SIZE - i);
    return PopCount::count(x);
}


// Pre-calculated select value table.
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

FORCE_INLINE uint64_t select64(uint64_t block, uint64_t i, uint64_t base) {
    // Calculate hamming weight
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

    // Get leading zero length
#if defined(HSDS_USE_POPCNT) && defined(HSDS_USE_SSE3)
    uint8_t leading_zero_len;
    {
        __m128i x = _mm_cvtsi64_si128((i + 1) * MASK_01);
        __m128i y = _mm_cvtsi64_si128(counts);
        x = _mm_cmpgt_epi8(x, y);
        leading_zero_len = (uint8_t)PopCount::count(_mm_cvtsi128_si64(x));
    }
#else // defined(HSDS_USE_POPCNT)
    const uint64_t x = (counts | MASK_80) - ((i + 1) * MASK_01);
#if defined(_MSC_VER)
    unsigned long leading_zero_len;
    ::_BitScanForward64(&leading_zero_len, (x & MASK_80) >> 7);
    --leading_zero_len;
#else // defined(_MSC_VER)
    const int leading_zero_len = ::__builtin_ctzll((x & MASK_80) >> 7);
#endif // defined(_MSC_VER)
#endif // defined(HSDS_USE_POPCNT)
    base += leading_zero_len;
    block >>= leading_zero_len;
    i -= ((counts << 8) >> leading_zero_len) & 0xFF;
    return base + SELECT_TABLE[i][block & 0xFF];
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
    blocks_type().swap(blocks_);
    ranktable_type().swap(rank_table_);
    select_dict_type().swap(select0_table_);
    select_dict_type().swap(select1_table_);
    size_ = 0;
    num_of_1s_ = 0;
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
    rank_table_.clear();
    uint64_t block_num = blocks_.size();
    uint64_t num_0s_in_lblock = L_BLOCK_SIZE;
    uint64_t num_1s_in_lblock = L_BLOCK_SIZE;
    num_of_1s_ = 0;

    ranktable_type().swap(rank_table_);
    rank_table_.resize(
            ((block_num * S_BLOCK_SIZE) / L_BLOCK_SIZE) + (((block_num * S_BLOCK_SIZE) % L_BLOCK_SIZE) != 0 ? 1 : 0)
                    + 1);
    select_dict_type().swap(select0_table_);
    select_dict_type().swap(select1_table_);

    for (uint64_t i = 0; i < block_num; ++i) {
        uint64_t rank_id = i / BLOCK_RATE;
        RankIndex &rank = rank_table_[rank_id];
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
            uint32_t pos = select64(blocks_[i], diff, 0);
            select1_table_.push_back(i * S_BLOCK_SIZE + pos);
            num_1s_in_lblock -= L_BLOCK_SIZE;
        }
        uint64_t count0s = S_BLOCK_SIZE - count1s;
        if (num_0s_in_lblock + count0s > L_BLOCK_SIZE) {
            uint32_t diff = L_BLOCK_SIZE - num_0s_in_lblock;
            uint32_t pos = select64(~blocks_[i], diff, 0);
            select0_table_.push_back(i * S_BLOCK_SIZE + pos);
            num_0s_in_lblock -= L_BLOCK_SIZE;
        }

        num_1s_in_lblock += count1s;
        num_0s_in_lblock += count0s;
        num_of_1s_ += count1s;
    }

    if ((block_num % BLOCK_RATE) != 0) {
        uint64_t rank_id = (block_num - 1) / BLOCK_RATE;
        RankIndex &rank = rank_table_[rank_id];
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
    rank_table_.back().set_abs(num_of_1s_);
    select0_table_.push_back(size_);
    select1_table_.push_back(size_);
}

uint64_t BitVector::rank(uint64_t i) const throw (hsds::Exception) {
    HSDS_EXCEPTION_IF(i > this->size(), "Out of range access");
    uint64_t q_large = i / L_BLOCK_SIZE;
    uint64_t q_small = i / S_BLOCK_SIZE;
    uint64_t r = i % S_BLOCK_SIZE;

    const RankIndex &rank = rank_table_[q_large];
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

uint64_t BitVector::select0(uint64_t x) const throw (hsds::Exception) {
    HSDS_EXCEPTION_IF(x >= this->size(false), "Out of range access");

    const uint64_t select_id = x / L_BLOCK_SIZE;
    if ((x % L_BLOCK_SIZE) == 0) {
        return select0_table_[select_id];
    }
    uint64_t begin = select0_table_[select_id] / L_BLOCK_SIZE;
    uint64_t end = (select0_table_[select_id + 1] + L_BLOCK_SIZE - 1) / L_BLOCK_SIZE;
    if (begin + 10 >= end) {
        while (x >= ((begin + 1) * L_BLOCK_SIZE) - rank_table_[begin + 1].abs()) {
            ++begin;
        }
    } else {
        while (begin + 1 < end) {
            const uint64_t pivot = (begin + end) / 2;
            if (x < (pivot * L_BLOCK_SIZE) - rank_table_[pivot].abs()) {
                end = pivot;
            } else {
                begin = pivot;
            }
        }
    }

    uint64_t rank_id = begin;
    const RankIndex &rank = rank_table_[rank_id];
    x -= (rank_id * L_BLOCK_SIZE) - rank.abs();
    uint64_t block_id = rank_id * BLOCK_RATE;
    if (x < (256U - rank.rel4())) {
        if (x < (128U - rank.rel2())) {
            if (x >= (64U - rank.rel1())) {
                block_id += 1;
                x -= 64 - rank.rel1();
            }
        } else if (x < (192U - rank.rel3())) {
            block_id += 2;
            x -= 128 - rank.rel2();
        } else {
            block_id += 3;
            x -= 192 - rank.rel3();
        }
    } else if (x < (384U - rank.rel6())) {
        if (x < (320U - rank.rel5())) {
            block_id += 4;
            x -= 256 - rank.rel4();
        } else {
            block_id += 5;
            x -= 320 - rank.rel5();
        }
    } else if (x < (448U - rank.rel7())) {
        block_id += 6;
        x -= 384 - rank.rel6();
    } else {
        block_id += 7;
        x -= 448 - rank.rel7();
    }
    return select64(~blocks_[block_id], x, block_id * S_BLOCK_SIZE);
}

uint64_t BitVector::select1(uint64_t x) const throw (hsds::Exception) {
    HSDS_EXCEPTION_IF(x >= this->size(true), "Out of range access");

    const uint64_t select_id = x / L_BLOCK_SIZE;
    if ((x % L_BLOCK_SIZE) == 0) {
        return select1_table_[select_id];
    }
    uint64_t begin = select1_table_[select_id] / L_BLOCK_SIZE;
    uint64_t end = (select1_table_[select_id + 1] + L_BLOCK_SIZE - 1) / L_BLOCK_SIZE;

    if (begin + 10 >= end) {
        // Linear search in rank table
        while (x >= rank_table_[begin + 1].abs()) {
            ++begin;
        }
    } else {
        // Binary search in rank table
        while (begin + 1 < end) {
            uint64_t pivot = (begin + end) / 2;
            if (x < rank_table_[pivot].abs()) {
                end = pivot;
            } else {
                begin = pivot;
            }
        }
    }

    uint64_t rank_id = begin;

    const RankIndex &rank = rank_table_[rank_id];
    x -= rank.abs();
    uint64_t block_id = rank_id * BLOCK_RATE;

    if (x < rank.rel4()) {
        if (x < rank.rel2()) {
            if (x >= rank.rel1()) {
                block_id += 1;
                x -= rank.rel1();
            }
        } else if (x < rank.rel3()) {
            block_id += 2;
            x -= rank.rel2();
        } else {
            block_id += 3;
            x -= rank.rel3();
        }
    } else if (x < rank.rel6()) {
        if (x < rank.rel5()) {
            block_id += 4;
            x -= rank.rel4();
        } else {
            block_id += 5;
            x -= rank.rel5();
        }
    } else if (x < rank.rel7()) {
        block_id += 6;
        x -= rank.rel6();
    } else {
        block_id += 7;
        x -= rank.rel7();
    }

    return select64(blocks_[block_id], x, block_id * S_BLOCK_SIZE);
}

void BitVector::save(std::ostream &os) const {
    os.write((char*) &size_, sizeof(size_));
    os.write((char*) &num_of_1s_, sizeof(num_of_1s_));

    uint64_t size = blocks_.size();
    os.write((char*) &size, sizeof(size));
    os.write((char*) &blocks_[0], sizeof(blocks_[0]) * size);

    size = rank_table_.size();
    os.write((char*) &size, sizeof(size));
    os.write((char*) &rank_table_[0], sizeof(rank_table_[0]) * size);

    size = select0_table_.size();
    os.write((char*) &size, sizeof(size));
    os.write((char*) &select0_table_[0], sizeof(select0_table_[0]) * size);

    size = select1_table_.size();
    os.write((char*) &size, sizeof(size));
    os.write((char*) &select1_table_[0], sizeof(select1_table_[0]) * size);

    HSDS_EXCEPTION_IF(os.fail(), "Failed to save the bit vector.");
}

void BitVector::load(std::istream &is) {
    is.read((char*) &size_, sizeof(size_));
    HSDS_EXCEPTION_IF( (is.eof() || is.fail() ), "Failed to read file. File format is invalid.");

    is.read((char*) &num_of_1s_, sizeof(num_of_1s_));
    HSDS_EXCEPTION_IF( (is.eof() || is.fail() ), "Failed to read file. File format is invalid.");

    uint64_t size = 0;
    is.read((char*) &size, sizeof(size));
    HSDS_EXCEPTION_IF( (is.eof() || is.fail() ), "Failed to read file. File format is invalid.");

    blocks_type().swap(blocks_);
    blocks_.resize(size);
    is.read((char*) &blocks_[0], sizeof(blocks_[0]) * size);
    HSDS_EXCEPTION_IF( (is.eof() || is.fail() ), "Failed to read file. File format is invalid.");

    is.read((char*) &size, sizeof(size));
    HSDS_EXCEPTION_IF( (is.eof() || is.fail() ), "Failed to read file. File format is invalid.");

    ranktable_type().swap(rank_table_);
    rank_table_.resize(size);
    is.read((char*) &rank_table_[0], sizeof(rank_table_[0]) * size);
    HSDS_EXCEPTION_IF( (is.eof() || is.fail() ), "Failed to read file. File format is invalid.");

    is.read((char*) &size, sizeof(size));
    HSDS_EXCEPTION_IF( (is.eof() || is.fail() ), "Failed to read file. File format is invalid.");

    select_dict_type().swap(select0_table_);
    select0_table_.resize(size);
    is.read((char*) &select0_table_[0], sizeof(select0_table_[0]) * size);
    HSDS_EXCEPTION_IF( (is.eof() || is.fail() ), "Failed to read file. File format is invalid.");

    is.read((char*) &size, sizeof(size));
    HSDS_EXCEPTION_IF( (is.eof() || is.fail() ), "Failed to read file. File format is invalid.");

    select_dict_type().swap(select1_table_);
    select1_table_.resize(size);
    is.read((char*) &select1_table_[0], sizeof(select1_table_[0]) * size);
    HSDS_EXCEPTION_IF( is.fail(), "Failed to read file. File format is invalid.");
}

}
