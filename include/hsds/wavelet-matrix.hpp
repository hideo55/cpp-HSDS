/*
 * wavelet-matrix.hpp
 *
 *  Created on: 2014/05/25
 *      Author: Hideaki Ohno
 */

#if !defined(HSDS_WAVELET_MATRIX_HPP_)
#define HSDS_WAVELET_MATRIX_HPP_

#if !defined(_MSC_VER)
#include <stdint.h>
#endif // !defined(_MSC_VER)
#include <vector>
#include "hsds/scoped_ptr.hpp"
#include "hsds/bit-vector.hpp"
#include "hsds/vector.hpp"

namespace hsds {

bool uint2bit(uint64_t c, uint64_t i) {
    return ((c >> (sizeof(uint64_t) * 8 - 1 - i)) & 0x1ULL) == 0x1ULL;
}

// forward declaration
class Exception;
template<typename T>
class Vector;

class WaveletMatrix {
public:

    /**
     * Constructor
     */
    WaveletMatrix();

    /**
     * Destructor
     */
    virtual ~WaveletMatrix();

    void clear();

    void swap(WaveletMatrix& x);

    void build(std::vector<uint64_t>& src);

    inline uint64_t size() const {
        return size_;
    }

    inline uint64_t bitSize() const {
        return bitSize_;
    }

    uint64_t lookup(uint64_t pos) const;

    uint64_t rank(uint64_t c, uint64_t pos) const;

    uint64_t rankLessThan(uint64_t c, uint64_t pos) const;

    uint64_t rankMoreThan(uint64_t c, uint64_t pos) const;

    void rankAll(uint64_t c, uint64_t begin_pos, uint64_t end_pos, uint64_t& rank, uint64_t& rank_less_than,
            uint64_t& rank_more_than) const;

    uint64_t select(uint64_t c, uint64_t rank) const;

    uint64_t selectFromPos(uint64_t c, uint64_t pos, uint64_t rank) const;

    uint64_t freq(uint64_t c) const;

    uint64_t freqSum(uint64_t min_c, uint64_t max_c) const;

    uint64_t freqRange(uint64_t min_c, uint64_t max_c, uint64_t begin_pos, uint64_t end_pos) const;

    void maxRange(uint64_t begin_pos, uint64_t end_pos, uint64_t& pos, uint64_t& val) const;

    void minRange(uint64_t begin_pos, uint64_t end_pos, uint64_t& pos, uint64_t& val) const;

    void quantileRange(uint64_t begin_pos, uint64_t end_pos, uint64_t k, uint64_t& pos, uint64_t& val) const;

    void save(std::ostream& os) const;

    void load(std::istream& is);
private:
    typedef hsds::Vector<hsds::BitVector> bv_type;
    typedef hsds::Vector<hsds::Vector<uint64_t> > range_type;
    typedef hsds::Vector<uint64_t> uint64_vector_type;

    uint64_t size_;
    const uint64_t bitSize_;
    uint64_t alphabetNum_;
    uint64_t alphabetBitNum_;
    bv_type bv_;
    range_type nodeBeginPos_;
    uint64_vector_type seps_;

    uint64_t getAlphabetNum(const std::vector<uint64_t>& array) const;
    uint64_t log2(uint64_t x) const;
};

}

#endif /* !defined(HSDS_WAVELET_MATRIX_HPP_) */
