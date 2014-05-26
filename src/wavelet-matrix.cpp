/*
 * wavelet-matrix.cpp
 *
 *  Created on: 2014/05/25
 *      Author: Hideaki Ohno
 */

#include <vector>
#include "hsds/wavelet-matrix.hpp"

namespace hsds {

using namespace std;

WaveletMatrix::WaveletMatrix() :
        size_(0), bitSize_(sizeof(uint64_t) * 8), alphabet_num_(0), alphabet_bit_num_(0) {
}

WaveletMatrix::~WaveletMatrix() {
}

void WaveletMatrix::clear() {
    Vector<BitVector>().swap(bv_);
    Vector<Vector<uint64_t> >().swap(nodeBeginPos_);
    Vector<uint64_t>().swap(seps_);
    size_ = 0;
    alphabet_num_ = 0;
    alphabet_bit_num_ = 0;
}

void WaveletMatrix::build(vector<uint64_t>& src) {
    clear();
    alphabet_num_ = getAlphabetNum(src);
    alphabet_bit_num_ = log2(alphabet_num_);

    size_ = static_cast<uint64_t>(src.size());
    nodeBeginPos_.resize(alphabet_bit_num_);

    Vector<uint64_t> dummy;
    dummy.push_back(0);
    dummy.push_back(size_);
    Vector<uint64_t>* prev_begin_pos = &dummy;

    for (uint64_t i = 0; i < alphabet_bit_num_; ++i) {
        nodeBeginPos_[i].resize(1 << (i + 1));

        for (uint64_t j = 0; j < size_; ++j) {
            int bit = (src[j] >> (alphabet_bit_num_ - i - 1)) & 1;
            uint64_t subscript = src[j] >> (alphabet_bit_num_ - i);
            bv_[i].set((*prev_begin_pos)[subscript]++, bit);
            ++nodeBeginPos_[i][(subscript << 1) | bit];
        }

        uint64_t cur_max = (uint64_t) 1 << i;
        uint64_t rev = cur_max - 1;
        uint64_t prev_rev = cur_max - 1;

        // bit-reversed reverse loop (skipping the first one).
        // ex. 0111 -> 1011 -> 0011 -> 1101 -> ... -> 1000 -> 0000
        // http://musicdsp.org/showone.php?id=171
        for (uint64_t j = cur_max - 1; j > 0; --j) {
            rev ^= cur_max - (cur_max / 2 / (j & -j));
            (*prev_begin_pos)[prev_rev] = (*prev_begin_pos)[rev];
            prev_rev = rev;
        }
        (*prev_begin_pos)[0] = 0;

        cur_max <<= 1;
        rev = 0;
        uint64_t sum = 0;

        // bit-reversed loop.
        // ex. 0000 -> 1000 -> 0100 -> 1100 -> 0010 -> ... -> 0111 -> 1111
        // http://musicdsp.org/showone.php?id=171
        for (uint64_t j = 0; j < cur_max; ++j, rev ^= cur_max - (cur_max / 2 / (j & -j))) {
            uint64_t t = nodeBeginPos_[i][rev];
            nodeBeginPos_[i][rev] = sum;
            sum += t;
        }

        bv_[i].build();
        prev_begin_pos = &(nodeBeginPos_[i]);
    }
}

uint64_t WaveletMatrix::lookup(uint64_t pos) const {
    HSDS_EXCEPTION_IF(pos >= size_, E_OUT_OF_RANGE);

    uint64_t index = pos;
    uint64_t c = 0;

    for (size_t i = 0; i < bv_.size(); ++i) {
        const BitVector& bv = bv_[i];
        uint64_t bit = bv[index];
        c <<= 1;
        c |= bit;

        if (bit) {
            index = bv.rank1(index);
            index += nodeBeginPos_[i][1];
        } else {
            index = bv.rank0(index);
        }
    }
    return c;
}

uint64_t WaveletMatrix::rank(uint64_t c, uint64_t pos) const {
    HSDS_EXCEPTION_IF(pos >= size_, E_OUT_OF_RANGE);

    if (pos == 0) {
        return 0;
    }

    return 0;
}

uint64_t WaveletMatrix::select(uint64_t c, uint64_t rank) const {
    return 0;
}

uint64_t WaveletMatrix::selectFromPos(uint64_t c, uint64_t pos, uint64_t rank) const {
    return 0;
}

uint64_t WaveletMatrix::rankLessThan(uint64_t c, uint64_t pos) const {
    return 0;
}

uint64_t WaveletMatrix::rankMoreThan(uint64_t c, uint64_t pos) const {
    return 0;
}

void WaveletMatrix::rankAll(uint64_t c, uint64_t pos, uint64_t& rank, uint64_t& freqLessThan,
        uint64_t& freqkMoreThan) const {
}

uint64_t WaveletMatrix::freq(uint64_t c) const {
    return 0;
}

uint64_t WaveletMatrix::freqRange(uint64_t minC, uint64_t maxC, uint64_t begPos, uint64_t endPos) const {
    return 0;
}

void WaveletMatrix::maxRange(uint64_t begPos, uint64_t endPos, uint64_t& pos, uint64_t& val) const {
}

void WaveletMatrix::minRange(uint64_t begPos, uint64_t endPos, uint64_t& pos, uint64_t& val) const {
}

void WaveletMatrix::quantileRange(uint64_t begPos, uint64_t endPos, uint64_t k, uint64_t& pos, uint64_t& val) const {
}

void WaveletMatrix::listModeRange(uint64_t minC, uint64_t maxC, uint64_t begPos, uint64_t endPos, uint64_t num,
        std::vector<ListResult>& res) const {
}

void WaveletMatrix::listMinRange(uint64_t minC, uint64_t maxC, uint64_t begPos, uint64_t endPos, uint64_t num,
        std::vector<ListResult>& res) const {
}

void WaveletMatrix::listMaxRange(uint64_t minC, uint64_t maxC, uint64_t begPos, uint64_t endPos, uint64_t num,
        std::vector<ListResult>& res) const {
}

void WaveletMatrix::save(std::istream& is) const {
}

void WaveletMatrix::load(std::ostream& os) {
}

uint64_t WaveletMatrix::getAlphabetNum(const std::vector<uint64_t>& array) const {
    uint64_t alphabet_num = 0;
    for (size_t i = 0; i < array.size(); ++i) {
        if (array[i] >= alphabet_num) {
            alphabet_num = array[i] + 1;
        }
    }
    return alphabet_num;
}

uint64_t WaveletMatrix::log2(uint64_t x) const {
    if (x == 0)
        return 0;
    x--;
    uint64_t bit_num = 0;
    while (x >> bit_num) {
        ++bit_num;
    }
    return bit_num;
}

} // namespace hsds
