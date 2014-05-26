/*
 * wavelet-matrix.hpp
 *
 *  Created on: 2014/05/25
 *      Author: Hideaki Ohno
 */

#if !defined(HSDS_WAVELET_MATRIX_HPP_)
#define HSDS_WAVELET_MATRIX_HPP_

#include <stdint.h>
#include "hsds/vector.hpp"
#include "hsds/bit-vector.hpp"

namespace hsds {

const int BIT_REVERSE_TABLE_MAX_BITS = 16;

bool uint2bit(uint64_t c, uint64_t i) {
    return ((c >> (sizeof(uint64_t) * 8 - 1 - i)) & 0x1ULL) == 0x1ULL;
}

struct ListResult {
    ListResult(uint64_t c, uint64_t freq) :
            c(c), freq(freq) {
    }
    uint64_t c;
    uint64_t freq;
    bool operator <(const ListResult& lr) const {
        if (c != lr.c)
            return c < lr.c;
        return freq < lr.freq;
    }
};

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

    void build(std::vector<uint64_t>& src);

    inline uint64_t size() const {
        return size_;
    }

    inline uint64_t bitSize() const {
    return bitSize_;
    }

    uint64_t lookup(uint64_t pos) const;

    uint64_t rank(uint64_t c, uint64_t pos) const;

    uint64_t select(uint64_t c, uint64_t rank) const;

    uint64_t selectFromPos(uint64_t c, uint64_t pos, uint64_t rank) const;

    uint64_t rankLessThan(uint64_t c, uint64_t pos) const;

    uint64_t rankMoreThan(uint64_t c, uint64_t pos) const;

    void rankAll(uint64_t c, uint64_t pos, uint64_t& rank, uint64_t& freqLessThan, uint64_t& freqkMoreThan) const;

    uint64_t freq(uint64_t c) const;

    uint64_t freqRange(uint64_t minC, uint64_t maxC, uint64_t begPos, uint64_t endPos) const;

    void maxRange(uint64_t begPos, uint64_t endPos, uint64_t& pos, uint64_t& val) const;

    void minRange(uint64_t begPos, uint64_t endPos, uint64_t& pos, uint64_t& val) const;

    void quantileRange(uint64_t begPos, uint64_t endPos, uint64_t k, uint64_t& pos, uint64_t& val) const;

    /**
     * List the distinct characters appeared in A[begPos ... endPos) from most frequent ones
     */
    void listModeRange(uint64_t minC, uint64_t maxC, uint64_t begPos, uint64_t endPos, uint64_t num,
            std::vector<ListResult>& res) const;

    /**
     * List the distinct characters in A[begPos ... endPos) minC <= c < maxC  from smallest ones
     * @param minC  The smallerest character to be examined
     * @param maxC The uppker bound of the character to be examined
     * @param begPos The beginning position of the array (inclusive)
     * @param endPos The ending positin of the array (not inclusive)
     * @param num The maximum number of reporting results.
     * @param res The distinct chracters in the A[beg_pos ... end_pos) from smallest ones.
     *            Each item consists of c:character and freq: frequency of c.
     */
    void listMinRange(uint64_t minC, uint64_t maxC, uint64_t begPos, uint64_t endPos, uint64_t num,
            std::vector<ListResult>& res) const;

    /**
     * List the distinct characters appeared in A[begPos ... endPos) from largest ones
     * @param minC The smallerest character to be examined
     * @param maxC The uppker bound of the character to be examined
     * @param begPos The beginning position of the array (inclusive)
     * @param endPos The ending positin of the array (not inclusive)
     * @param num The maximum number of reporting results.
     * @param res The distinct chracters in the A[beg_pos ... end_pos) from largestx ones.
     *            Each item consists of c:character and freq: frequency of c.
     */
    void listMaxRange(uint64_t minC, uint64_t maxC, uint64_t begPos, uint64_t endPos, uint64_t num,
            std::vector<ListResult>& res) const;

    void save(std::istream& is) const;

    void load(std::ostream& os);
private:
    uint64_t size_;
    const uint64_t bitSize_;
    uint64_t alphabet_num_;
    uint64_t alphabet_bit_num_;
    hsds::Vector<hsds::BitVector> bv_;
    hsds::Vector<hsds::Vector<uint64_t> >nodeBeginPos_;
    hsds::Vector<uint64_t> seps_;

    uint64_t getAlphabetNum(const std::vector<uint64_t>& array) const;
    uint64_t log2(uint64_t x) const;
};

}

#endif /* !defined(HSDS_WAVELET_MATRIX_HPP_) */
