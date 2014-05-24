/*
 * wavelet-matrix.hpp
 *
 *  Created on: 2014/05/25
 *      Author: Hideaki Ohno
 */

#if !defined(HSDS_WAVELET_MATRIX_HPP_)
#define HSDS_WAVELET_MATRIX_HPP_

namespace hsds {

const int BIT_REVERSE_TABLE_MAX_BITS = 16;
const uint64_t NOT_FOUND = 0xFFFFFFFFFFFFFFFFLLU;

template<typename T>
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

    void build(std::vector<T>& src);

    inline uint64_t size() const {
        return size_;
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

private:
    uint64_t size_;
    hsds::Vector<hsds::BitVector> bv_;
    hsds::Vector<hsds::Vector<uint64_t> > nodeBeginPos_;
    hsds::Vector<uint64_t> zeros_;
    hsds::Vector<uint64_t> reverse_;
};

}

#endif /* !defined(HSDS_WAVELET_MATRIX_HPP_) */
