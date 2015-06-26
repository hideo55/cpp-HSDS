/**
 * @file bit-vector.cpp
 * @brief Implementation of BItVector
 * @author Hideaki Ohno
 */
#include "hsds/bit-vector.hpp"
#include "hsds/internal/popcount.hpp"
#include "hsds/exception.hpp"
#include <algorithm>

namespace hsds {
using namespace std;

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

const uint64_t MASK_01 = 0x0101010101010101ULL;
#if !defined(HSDS_USE_SSE3)
const uint64_t MASK_55 = 0x5555555555555555ULL;
const uint64_t MASK_33 = 0x3333333333333333ULL;
const uint64_t MASK_0F = 0x0F0F0F0F0F0F0F0FULL;
#endif
#if !(defined(HSDS_USE_POPCNT) && defined(HSDS_USE_SSE3)) && defined(_MSC_VER)
const uint64_t MASK_80 = 0x8080808080808080ULL;
#endif

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
    uint8_t trailing_zero_len;
    {
        __m128i x = _mm_cvtsi64_si128((i + 1) * MASK_01);
        __m128i y = _mm_cvtsi64_si128(counts);
        x = _mm_cmpgt_epi8(x, y);
        trailing_zero_len = (uint8_t)PopCount::count(_mm_cvtsi128_si64(x));
    }
#else // defined(HSDS_USE_POPCNT)
    const uint64_t x = (counts | MASK_80) - ((i + 1) * MASK_01);
#if defined(_MSC_VER)
    unsigned long trailing_zero_len;
    ::_BitScanForward64(&trailing_zero_len, (x & MASK_80) >> 7);
    --trailing_zero_len;
#else // defined(_MSC_VER)
    const int trailing_zero_len = ::__builtin_ctzll((x & MASK_80) >> 7);
#endif // defined(_MSC_VER)
#endif // defined(HSDS_USE_POPCNT)
    base += trailing_zero_len;
    block >>= trailing_zero_len;
    i -= ((counts << 8) >> trailing_zero_len) & 0xFF;
    return base + SELECT_TABLE[i][block & 0xFF];
}

FORCE_INLINE uint64_t mask(uint64_t x, uint64_t pos){
  return x & ((1LLU << pos) - 1);
}

BitVector::BitVector() :
        size_(0), num_of_1s_(0), freeze_(false) {
}

BitVector::BitVector(uint64_t size) :
        size_(size), num_of_1s_(0), freeze_(false) {
    uint64_t block_num = (size + S_BLOCK_SIZE - 1) / S_BLOCK_SIZE;
    blocks_.resize(block_num, 0);
}

BitVector::~BitVector() {}

bool BitVector::operator[](uint64_t i) const {
    HSDS_DEBUG_IF(i >= size_, E_OUT_OF_RANGE);
    return (blocks_[i / S_BLOCK_SIZE] & (1ULL << (i % S_BLOCK_SIZE))) != 0;
}

void BitVector::set(uint64_t i, bool b) {
    HSDS_EXCEPTION_IF(freeze_, E_FREEZE);
    if (i >= size_) {
        size_ = i + 1;
    }
    uint64_t block_id = i / S_BLOCK_SIZE;
    uint64_t r = i % S_BLOCK_SIZE;

    if (block_id >= blocks_.size()) {
        blocks_.resize(block_id + 1, 0);
    }

    uint64_t m = 0x1ULL << r;
    if (b) {
        blocks_[block_id] |= m;
    } else {
        blocks_[block_id] &= ~m;
    }
}

void BitVector::push_back(bool b) {
    HSDS_EXCEPTION_IF(freeze_, E_FREEZE);
    if(size_/S_BLOCK_SIZE >= blocks_.size()) {
        blocks_.push_back(0);
    }
    
    uint64_t block_id = size_ / S_BLOCK_SIZE;
    uint64_t r = size_ % S_BLOCK_SIZE;
    uint64_t m = 0x1ULL << r;
    if(b) {
        blocks_[block_id] |= m;
    } else {
        blocks_[block_id] &= ~m;
    }
    ++size_;
}

void BitVector::push_back_bits(uint64_t x, uint64_t len) {
    HSDS_EXCEPTION_IF(freeze_, E_FREEZE);
    size_t offset = size_ % S_BLOCK_SIZE;
    if ((size_ + len - 1) / S_BLOCK_SIZE >= blocks_.size()){
      blocks_.push_back(0);
    }

    blocks_[size_ / S_BLOCK_SIZE] |= (x << offset);
    if (offset + len - 1 >= S_BLOCK_SIZE){
      blocks_[size_ / S_BLOCK_SIZE + 1] |= (x >> (S_BLOCK_SIZE - offset));
    }
    size_ += len;
}

uint64_t BitVector::get_bits(uint64_t pos, uint64_t len) const {
    uint64_t blockInd1    = pos / S_BLOCK_SIZE;
    uint64_t blockOffset1 = pos % S_BLOCK_SIZE;
    if (blockOffset1 + len <= S_BLOCK_SIZE){
      return mask(blocks_[blockInd1] >> blockOffset1, len);
    } else {
      uint64_t blockInd2    = ((pos + len - 1) / S_BLOCK_SIZE);
      return  mask((blocks_[blockInd1] >> blockOffset1) + (blocks_[blockInd2] << (S_BLOCK_SIZE - blockOffset1)), len);
    }
}

void BitVector::build(bool enable_faster_select1, bool enable_faster_select0) {
    uint64_t block_num = blocks_.size();
    uint64_t num_0s_in_lblock = L_BLOCK_SIZE;
    uint64_t num_1s_in_lblock = L_BLOCK_SIZE;
    num_of_1s_ = 0;

    rank_dict_type().swap(rank_table_);
    select_dict_type().swap(select0_table_);
    select_dict_type().swap(select1_table_);

    rank_table_.resize(
            ((block_num * S_BLOCK_SIZE) / L_BLOCK_SIZE) + (((block_num * S_BLOCK_SIZE) % L_BLOCK_SIZE) != 0 ? 1 : 0)
                    + 1);

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

        if (enable_faster_select1 && (num_1s_in_lblock + count1s > L_BLOCK_SIZE)) {
            uint32_t diff = L_BLOCK_SIZE - num_1s_in_lblock;
            uint32_t pos = select64(blocks_[i], diff, 0);
            select1_table_.push_back(i * S_BLOCK_SIZE + pos);
            num_1s_in_lblock -= L_BLOCK_SIZE;
        }
        uint64_t count0s = S_BLOCK_SIZE - count1s;
        if (enable_faster_select0 && (num_0s_in_lblock + count0s > L_BLOCK_SIZE)) {
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

    if (enable_faster_select1) {
        select1_table_.push_back(size_);
    }

    if (enable_faster_select0) {
        select0_table_.push_back(size_);
    }
    freeze_ = true;
}

uint64_t BitVector::rank0(uint64_t i) const {
    if (i > size()) {
        return NOT_FOUND;
    }
    return i - rank1(i);
}

uint64_t BitVector::rank1(uint64_t i) const {
    if (i > size()) {
        return NOT_FOUND;
    }
    uint64_t rank_id = i / L_BLOCK_SIZE;
    uint64_t block_id = i / S_BLOCK_SIZE;
    uint64_t r = i % S_BLOCK_SIZE;

    const RankIndex &rank = rank_table_[rank_id];
    uint64_t offset = rank.abs();
    switch (block_id % BLOCK_RATE) {
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
    offset += PopCount::count(blocks_[block_id] & ((1ULL << r) - 1));
    return offset;
}

uint64_t BitVector::select0(uint64_t x) const {
    if (x >= size(false)) {
        return NOT_FOUND;
    }

    uint64_t begin;
    uint64_t end;

    if (select0_table_.empty()) {
        begin = 0;
        end = rank_table_.size();
    } else {
        const uint64_t select_id = x / L_BLOCK_SIZE;
        if ((x % L_BLOCK_SIZE) == 0) {
            return select0_table_[select_id];
        }
        begin = select0_table_[select_id] / L_BLOCK_SIZE;
        end = (select0_table_[select_id + 1] + L_BLOCK_SIZE - 1) / L_BLOCK_SIZE;
    }

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

uint64_t BitVector::select1(uint64_t x) const {
    if (x >= size(true)) {
        return NOT_FOUND;
    }

    uint64_t begin;
    uint64_t end;

    if (select1_table_.empty()) {
        begin = 0;
        end = rank_table_.size();
    } else {
        const uint64_t select_id = x / L_BLOCK_SIZE;
        if ((x % L_BLOCK_SIZE) == 0) {
            return select1_table_[select_id];
        }
        begin = select1_table_[select_id] / L_BLOCK_SIZE;
        end = (select1_table_[select_id + 1] + L_BLOCK_SIZE - 1) / L_BLOCK_SIZE;
    }

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

void BitVector::save(std::ostream &os) const throw (hsds::Exception) {
    os.write(reinterpret_cast<const char*>(&size_), sizeof(size_));
    os.write(reinterpret_cast<const char*>(&num_of_1s_), sizeof(num_of_1s_));
    blocks_.save(os);
    rank_table_.save(os);
    select0_table_.save(os);
    select1_table_.save(os);

    HSDS_EXCEPTION_IF(os.fail(), E_SAVE_FILE);
}

void BitVector::load(std::istream &is) throw (hsds::Exception) {
    clear();
    is.read(reinterpret_cast<char*>(&size_), sizeof(size_));
    HSDS_EXCEPTION_IF((is.eof() || is.fail()), E_LOAD_FILE);

    is.read(reinterpret_cast<char*>(&num_of_1s_), sizeof(num_of_1s_));
    HSDS_EXCEPTION_IF((is.eof() || is.fail()), E_LOAD_FILE);

    blocks_.load(is);
    rank_table_.load(is);
    select0_table_.load(is);
    select1_table_.load(is);
    HSDS_EXCEPTION_IF(is.fail(), E_LOAD_FILE);
    freeze_ = true;
}

uint64_t BitVector::map(void* ptr, uint64_t mapSize) throw (hsds::Exception) {
    size_ = *(static_cast<uint64_t*>(ptr));
    uint64_t offset = sizeof(size_);
    HSDS_EXCEPTION_IF(offset >= mapSize, E_LOAD_FILE);

    num_of_1s_ = *(reinterpret_cast<uint64_t*>(reinterpret_cast<char*>(ptr) + offset));
    offset += sizeof(num_of_1s_);
    HSDS_EXCEPTION_IF(offset >= mapSize, E_LOAD_FILE);

    offset += blocks_.map(reinterpret_cast<char*>(ptr) + offset, mapSize - offset);
    HSDS_EXCEPTION_IF(offset >= mapSize, E_LOAD_FILE);

    offset += rank_table_.map(reinterpret_cast<char*>(ptr) + offset, mapSize - offset);
    HSDS_EXCEPTION_IF(offset >= mapSize, E_LOAD_FILE);

    offset += select0_table_.map(reinterpret_cast<char*>(ptr) + offset, mapSize - offset);
    HSDS_EXCEPTION_IF(offset >= mapSize, E_LOAD_FILE);

    offset += select1_table_.map(reinterpret_cast<char*>(ptr) + offset, mapSize - offset);
    HSDS_EXCEPTION_IF(offset > mapSize, E_LOAD_FILE);
    freeze_ = true;
    return offset;
}

void BitVector::swap(BitVector &x) {
    std::swap(size_, x.size_);
    std::swap(num_of_1s_, x.num_of_1s_);
    blocks_.swap(x.blocks_);
    rank_table_.swap(x.rank_table_);
    select0_table_.swap(x.select0_table_);
    select1_table_.swap(x.select1_table_);
    std::swap(freeze_, x.freeze_);
}

} // namespace hsds

