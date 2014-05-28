/*
 * wavelet-matrix.cpp
 *
 *  Created on: 2014/05/25
 *      Author: Hideaki Ohno
 */

#include "hsds/wavelet-matrix.hpp"
#include <algorithm>

namespace hsds {

using namespace std;

WaveletMatrix::WaveletMatrix() :
        size_(0), bitSize_(sizeof(uint64_t) * 8), alphabetNum_(0), alphabetBitNum_(0) {
}

WaveletMatrix::~WaveletMatrix() {
}

void WaveletMatrix::clear() {
    WaveletMatrix().swap(*this);
}

void WaveletMatrix::swap(WaveletMatrix& x) {
    std::swap(size_, x.size_);
    std::swap(alphabetNum_, x.alphabetNum_);
    std::swap(alphabetBitNum_, x.alphabetBitNum_);
    bv_.swap(x.bv_);
    nodeBeginPos_.swap(x.nodeBeginPos_);
    seps_.swap(x.seps_);
}

void WaveletMatrix::build(vector<uint64_t>& src) {
    clear();
    alphabetNum_ = getAlphabetNum(src);
    
    if(alphabetNum_ == 0) return;
    
    alphabetBitNum_ = log2(alphabetNum_);

    size_ = static_cast<uint64_t>(src.size());
    bv_.resize(alphabetBitNum_, size_);
    nodeBeginPos_.resize(alphabetBitNum_);

    Vector<uint64_t> dummy;
    dummy.push_back(0);
    dummy.push_back(size_);
    Vector<uint64_t>* prev_begin_pos = &dummy;

    for (uint64_t i = 0; i < alphabetBitNum_; ++i) {
        nodeBeginPos_[i].resize(1 << (i + 1), 0);

        for (uint64_t j = 0; j < size_; ++j) {
            int bit = (src[j] >> (alphabetBitNum_ - i - 1)) & 1;
            uint64_t subscript = src[j] >> (alphabetBitNum_ - i);
            bv_[i].set((*prev_begin_pos)[subscript]++, bit);
            ++nodeBeginPos_[i][(subscript << 1) | bit];
        }

        uint64_t cur_max = (uint64_t) 1 << i;
        uint64_t rev = cur_max - 1;
        uint64_t prev_rev = cur_max - 1;

        for (uint64_t j = cur_max - 1; j > 0; --j) {
            rev ^= cur_max - (cur_max / 2 / (j & -j));
            (*prev_begin_pos)[prev_rev] = (*prev_begin_pos)[rev];
            prev_rev = rev;
        }
        (*prev_begin_pos)[0] = 0;

        cur_max <<= 1;
        rev = 0;
        uint64_t sum = 0;

        for (uint64_t j = 0; j < cur_max; ++j, rev ^= cur_max - (cur_max / 2 / (j & -j))) {
            uint64_t t = nodeBeginPos_[i][rev];
            nodeBeginPos_[i][rev] = sum;
            sum += t;
        }

        bv_[i].build(true, true);
        prev_begin_pos = &(nodeBeginPos_[i]);
    }
}

uint64_t WaveletMatrix::lookup(uint64_t pos) const {
    if (pos >= size_)
        return hsds::NOT_FOUND;

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
    if (c >= alphabetNum_ || pos > size_) {
        return hsds::NOT_FOUND;
    }

    if (pos == 0) {
        return 0;
    }

    uint64_t beginPos = nodeBeginPos_[alphabetBitNum_ - 1][c];
    uint64_t endPos = pos;

    for (size_t i = 0; i < alphabetBitNum_; ++i) {
        const BitVector& bv = bv_[i];
        unsigned int bit = (c >> (alphabetBitNum_ - i - 1)) & 1;
        endPos = bit ? bv.rank1(endPos) : bv.rank0(endPos);
        if (bit) {
            endPos += nodeBeginPos_[i][1];
        }
    }
    return endPos - beginPos;

    return 0;
}

uint64_t WaveletMatrix::rankLessThan(uint64_t c, uint64_t pos) const {
    uint64_t rank_less_than = 0;
    uint64_t rank_more_than = 0;
    uint64_t rank = 0;
    rankAll(c, 0, pos, rank, rank_less_than, rank_more_than);
    return rank_less_than;
}

uint64_t WaveletMatrix::rankMoreThan(uint64_t c, uint64_t pos) const {
    uint64_t rank_less_than = 0;
    uint64_t rank_more_than = 0;
    uint64_t rank = 0;
    rankAll(c, 0, pos, rank, rank_less_than, rank_more_than);
    return rank_more_than;
}

void WaveletMatrix::rankAll(uint64_t c, uint64_t begin_pos, uint64_t end_pos, uint64_t& rank, uint64_t& rank_less_than,
        uint64_t& rank_more_than) const {
    if (c >= alphabetNum_ || begin_pos >= size_ || end_pos > size_) {
        rank_less_than = NOT_FOUND;
        rank_more_than = NOT_FOUND;
        rank = NOT_FOUND;
        return;
    }
    rank_less_than = 0;
    rank_more_than = 0;
    rank = 0;

    if (begin_pos >= end_pos) {
        return;
    }

    uint64_t more_and_less[2] =
    { 0 };
    uint64_t node_num = 0;
    bool from_zero = (begin_pos == 0);
    bool to_end = (end_pos == size_);

    for (size_t i = 0; i < alphabetBitNum_; ++i) {
        const BitVector& bv = bv_[i];
        unsigned int bit = (c >> (alphabetBitNum_ - i - 1)) & 1;
        uint64_t range_bits = end_pos - begin_pos;
        uint64_t begin_zero, end_zero;

        if (from_zero) {
            begin_zero = nodeBeginPos_[i][node_num];
        } else {
            begin_zero = bv.rank0(begin_pos);
        }

        if (to_end) {
            end_zero = nodeBeginPos_[i][node_num + 1];
        } else {
            end_zero = bv.rank0(end_pos);
        }

        if (bit) {
            begin_pos += nodeBeginPos_[i][1] - begin_zero;
            end_pos += nodeBeginPos_[i][1] - end_zero;
        } else {
            begin_pos = begin_zero;
            end_pos = end_zero;
        }
        more_and_less[bit] += range_bits - (end_pos - begin_pos);
        node_num |= bit << i;
    }
    rank_less_than = more_and_less[1];
    rank_more_than = more_and_less[0];
    rank = end_pos - begin_pos;
}

uint64_t WaveletMatrix::select(uint64_t c, uint64_t rank) const {
    return selectFromPos(c, 0, rank);
}

uint64_t WaveletMatrix::selectFromPos(uint64_t c, uint64_t pos, uint64_t rank) const {
    if (c >= alphabetNum_ || pos >= size_) {
        return NOT_FOUND;
    }

    uint64_t index;
    if (pos == 0) {
        index = nodeBeginPos_[alphabetBitNum_ - 1][c];
    } else {
        index = pos;
        for (uint64_t i = 0; i < alphabetBitNum_; ++i) {
            unsigned int bit = (c >> (alphabetBitNum_ - i - 1)) & 1;
            index = bit ? bv_[i].rank1(index) : bv_[i].rank0(index);
            if (bit) {
                index += nodeBeginPos_[i][1];
            }
        }
    }

    index += rank;

    for (int i = alphabetBitNum_ - 1; i >= 0; --i) {
        unsigned int bit = (c >> (alphabetBitNum_ - i - 1)) & 1;
        if (bit) {
            index -= nodeBeginPos_[i][1];
        }

        index = (bit ? bv_[i].select1(index) : bv_[i].select0(index)) + 1;
        if (index == hsds::NOT_FOUND) {
            return hsds::NOT_FOUND;
        }
    }
    return index;

}

uint64_t WaveletMatrix::freq(uint64_t c) const {
    return rank(c, size_);
}

uint64_t WaveletMatrix::freqSum(uint64_t min_c, uint64_t max_c) const {
    uint64_t sum = 0;
    for (uint64_t i = min_c; i < max_c; ++i) {
        sum += freq(i);
    }
    return sum;
}

uint64_t WaveletMatrix::freqRange(uint64_t min_c, uint64_t max_c, uint64_t begin_pos, uint64_t end_pos) const {
    if (min_c >= alphabetNum_)
        return 0;
    if (max_c <= min_c)
        return 0;
    if (end_pos > size_ || begin_pos >= end_pos)
        return 0;
    uint64_t rank, max_less, min_less, more;
    rankAll(max_c, begin_pos, end_pos, rank, max_less, more);
    rankAll(min_c, begin_pos, end_pos, rank, min_less, more);
    return max_less - min_less;
}

void WaveletMatrix::maxRange(uint64_t begin_pos, uint64_t end_pos, uint64_t& pos, uint64_t& val) const {
    quantileRange(begin_pos, end_pos, end_pos - begin_pos - 1, pos, val);
}

void WaveletMatrix::minRange(uint64_t begin_pos, uint64_t end_pos, uint64_t& pos, uint64_t& val) const {
    quantileRange(begin_pos, end_pos, 0, pos, val);
}

void WaveletMatrix::quantileRange(uint64_t begin_pos, uint64_t end_pos, uint64_t k, uint64_t& pos,
        uint64_t& val) const {
    if ((end_pos > size_ || begin_pos >= end_pos) || (k >= end_pos - begin_pos)) {
        pos = NOT_FOUND;
        val = NOT_FOUND;
        return;
    }

    val = 0;

    uint64_t node_num = 0;
    uint64_t begin_zero, end_zero;
    bool from_zero = (begin_pos == 0);
    bool to_end = (end_pos == size_);

    for (size_t i = 0; i < alphabetBitNum_; ++i) {
        const BitVector& bv = bv_[i];

        if (from_zero) {
            begin_zero = nodeBeginPos_[i][node_num];
        } else {
            begin_zero = bv.rank0(begin_pos);
        }

        if (to_end) {
            end_zero = nodeBeginPos_[i][node_num + 1];
        } else {
            end_zero = bv.rank0(end_pos);
        }

        uint64_t zero_bits = end_zero - begin_zero;
        unsigned int bit = (k < zero_bits) ? 0 : 1;

        if (bit) {
            k -= zero_bits;
            begin_pos += nodeBeginPos_[i][1] - begin_zero;
            end_pos += nodeBeginPos_[i][1] - end_zero;
        } else {
            begin_pos = begin_zero;
            end_pos = end_zero;
        }
        node_num |= bit << i;
        val <<= 1;
        val |= bit;
    }

    pos = select(val, begin_pos + k - nodeBeginPos_[alphabetBitNum_ - 1][val] + 1) - 1;

}

void WaveletMatrix::save(std::ostream& os) const {
    os.write((const char*) (&alphabetNum_), sizeof(alphabetNum_));
    os.write((const char*) (&size_), sizeof(size_));
    for (size_t i = 0; i < bv_.size(); ++i) {
        bv_[i].save(os);
    }
    for (size_t i = 0; i < bv_.size(); ++i) {
        for (size_t j = 0; j < (size_t) (1 << (i + 1)); ++j) {
            os.write((const char*) (&nodeBeginPos_[i][j]), sizeof(range_type));
        }
    }
}

void WaveletMatrix::load(std::istream& is) {
    clear();
    is.read((char*) (&alphabetNum_), sizeof(alphabetNum_));
    alphabetBitNum_ = log2(alphabetNum_);
    is.read((char*) (&size_), sizeof(size_));

    bv_.resize(alphabetBitNum_);
    for (size_t i = 0; i < bv_.size(); ++i) {
        bv_[i].load(is);
    }

    nodeBeginPos_.resize(bv_.size());
    for (size_t i = 0; i < bv_.size(); ++i) {
        nodeBeginPos_[i].resize(1 << (i + 1));
        for (size_t j = 0; j < (size_t) (1 << (i + 1)); ++j) {
            is.read((char*) (&nodeBeginPos_[i][j]), sizeof(nodeBeginPos_[i][j]));
        }
    }
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
