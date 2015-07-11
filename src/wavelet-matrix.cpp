/**
 * @file wavelet-matrix.cpp
 * @brief Implementation of WaveletMatrix
 * @author Hideaki Ohno
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
    nodePos_.swap(x.nodePos_);
    seps_.swap(x.seps_);
}

void WaveletMatrix::build(vector<uint64_t>& src) {
    clear();
    alphabetNum_ = getAlphabetNum(src);

    if (alphabetNum_ == 0)
        return;

    alphabetBitNum_ = log2(alphabetNum_);

    size_ = static_cast<uint64_t>(src.size());

    bv_.resize(alphabetBitNum_, size_);
    nodePos_.resize(alphabetBitNum_);

    Vector<uint64_t> dummy;
    dummy.push_back(0);
    dummy.push_back(size_);
    Vector<uint64_t>* prev_begin_pos = &dummy;

    for (uint64_t i = 0; i < alphabetBitNum_; ++i) {
        nodePos_[i].resize(1 << (i + 1), 0);

        for (uint64_t j = 0; j < size_; ++j) {
            int bit = (src[j] >> (alphabetBitNum_ - i - 1)) & 1;
            uint64_t subscript = src[j] >> (alphabetBitNum_ - i);
            bv_[i].set((*prev_begin_pos)[subscript]++, bit);
            ++nodePos_[i][(subscript << 1) | bit];
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
            uint64_t t = nodePos_[i][rev];
            nodePos_[i][rev] = sum;
            sum += t;
        }

        bv_[i].build(true, true);
        prev_begin_pos = &(nodePos_[i]);
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
        index = bv.rank(index, bit);
        if (bit) {
            index += nodePos_[i][1];
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

    uint64_t beginPos = nodePos_[alphabetBitNum_ - 1][c];
    uint64_t endPos = pos;

    for (size_t i = 0; i < alphabetBitNum_; ++i) {
        const BitVector& bv = bv_[i];
        unsigned int bit = (c >> (alphabetBitNum_ - i - 1)) & 1;
        endPos = bv.rank(endPos, bit);
        if (bit) {
            endPos += nodePos_[i][1];
        }
    }
    return endPos - beginPos;
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

    uint64_t beg_node = 0;
    uint64_t end_node = size_;
    uint64_t pos = end_pos;

    for (size_t i = 0; i < alphabetBitNum_ && begin_pos < end_pos; ++i) {
        const BitVector& bv = bv_[i];
        unsigned int bit = (c >> (alphabetBitNum_ - i - 1)) & 1;
        uint64_t beg_node_zero = bv.rank0(beg_node);
        uint64_t beg_node_one  = beg_node - beg_node_zero;
        uint64_t end_node_zero = bv.rank0(end_node);
        uint64_t boundary      = beg_node + end_node_zero - beg_node_zero;
        if (!bit){
          rank_more_than += bv.rank1(pos) - beg_node_one;
          pos      = beg_node + bv.rank0(pos) - beg_node_zero;
          end_node = boundary;
        } else {
          rank_less_than += bv.rank0(pos) - beg_node_zero;
          pos      = boundary + bv.rank1(pos) - (beg_node - beg_node_zero);
          beg_node = boundary;
        }
    }
    rank = pos - beg_node;
}

uint64_t WaveletMatrix::select(uint64_t c, uint64_t rank) const {
    return selectFromPos(c, 0, rank);
}

uint64_t WaveletMatrix::selectFromPos(uint64_t c, uint64_t pos, uint64_t rank) const {
    if (c >= alphabetNum_ || pos >= size_ || rank > freq(c)) {
        return NOT_FOUND;
    }

    uint64_t index;
    if (pos == 0) {
        index = nodePos_[alphabetBitNum_ - 1][c];
    } else {
        index = pos;
        for (uint64_t i = 0; i < alphabetBitNum_; ++i) {
            bool bit = (c >> (alphabetBitNum_ - i - 1)) & 1;
            index = bv_[i].rank(index, bit);
            if (bit) {
                index += nodePos_[i][1];
            }
        }
    }

    index += rank;

    for (int i = alphabetBitNum_ - 1; i >= 0; --i) {
        bool bit = (c >> (alphabetBitNum_ - i - 1)) & 1;
        if (bit) {
            index -= nodePos_[i][1];
        }

        index = bv_[i].select(index - 1, bit) + 1;

        if (index == hsds::NOT_FOUND) {
            return hsds::NOT_FOUND;
        }
    }
    return index - 1;

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
    if ((end_pos > size_ || begin_pos >= end_pos) || k >= end_pos - begin_pos) {
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
            begin_zero = nodePos_[i][node_num];
        } else {
            begin_zero = bv.rank0(begin_pos);
        }

        if (to_end) {
            end_zero = nodePos_[i][node_num + 1];
        } else {
            end_zero = bv.rank0(end_pos);
        }

        uint64_t zero_bits = end_zero - begin_zero;
        unsigned int bit = (k < zero_bits) ? 0 : 1;

        if (bit) {
            k -= zero_bits;
            begin_pos += nodePos_[i][1] - begin_zero;
            end_pos += nodePos_[i][1] - end_zero;
        } else {
            begin_pos = begin_zero;
            end_pos = end_zero;
        }
        node_num |= bit << i;
        val <<= 1;
        val |= bit;
    }

    pos = select(val, begin_pos + k - nodePos_[alphabetBitNum_ - 1][val] + 1);
}

class WaveletMatrix::ListModeComparator {
public:
    ListModeComparator() {
    }
    bool operator()(const QueryOnNode& lhs, const QueryOnNode& rhs) const {
        if (lhs.end_pos - lhs.beg_pos != rhs.end_pos - rhs.beg_pos) {
            return lhs.end_pos - lhs.beg_pos < rhs.end_pos - rhs.beg_pos;
        } else if (lhs.depth != rhs.depth) {
            return lhs.depth < rhs.depth;
        } else {
            return lhs.beg_pos > rhs.beg_pos;
        }
    }
};

class WaveletMatrix::ListMinComparator {
public:
    ListMinComparator() {
    }
    bool operator()(const QueryOnNode& lhs, const QueryOnNode& rhs) const {
        if (lhs.depth != rhs.depth)
            return lhs.depth < rhs.depth;
        else
            return lhs.beg_node > rhs.beg_node;
    }
};

class WaveletMatrix::ListMaxComparator {
public:
    ListMaxComparator() {
    }
    bool operator()(const QueryOnNode& lhs, const QueryOnNode& rhs) const {
        if (lhs.depth != rhs.depth)
            return lhs.depth < rhs.depth;
        else
            return lhs.beg_node < rhs.beg_node;
    }
};

void WaveletMatrix::listModeRange(uint64_t min_c, uint64_t max_c, uint64_t beg_pos, uint64_t end_pos, uint64_t num,
        std::vector<ListResult>& res) const {
    listRange<ListModeComparator>(min_c, max_c, beg_pos, end_pos, num, res);
}

void WaveletMatrix::listMinRange(uint64_t min_c, uint64_t max_c, uint64_t beg_pos, uint64_t end_pos, uint64_t num,
        std::vector<ListResult>& res) const {
    listRange<ListMinComparator>(min_c, max_c, beg_pos, end_pos, num, res);
}

void WaveletMatrix::listMaxRange(uint64_t min_c, uint64_t max_c, uint64_t beg_pos, uint64_t end_pos, uint64_t num,
        std::vector<ListResult>& res) const {
    listRange<ListMaxComparator>(min_c, max_c, beg_pos, end_pos, num, res);
}

void WaveletMatrix::save(std::ostream& os) const throw (hsds::Exception) {
    os.write(reinterpret_cast<const char*>(&alphabetNum_), sizeof(alphabetNum_));
    os.write(reinterpret_cast<const char*>(&size_), sizeof(size_));
    for (size_t i = 0; i < bv_.size(); ++i) {
        bv_[i].save(os);
    }
    for (size_t i = 0; i < bv_.size(); ++i) {
        nodePos_[i].save(os);
    }
}

void WaveletMatrix::load(std::istream& is) throw (hsds::Exception) {
    clear();
    is.read(reinterpret_cast<char*>(&alphabetNum_), sizeof(alphabetNum_));
    alphabetBitNum_ = log2(alphabetNum_);
    is.read(reinterpret_cast<char*>(&size_), sizeof(size_));

    bv_.resize(alphabetBitNum_);
    for (size_t i = 0; i < bv_.size(); ++i) {
        bv_[i].load(is);
    }

    nodePos_.resize(bv_.size());
    for (size_t i = 0; i < bv_.size(); ++i) {
        nodePos_[i].load(is);
    }
}

uint64_t WaveletMatrix::map(void* ptr, uint64_t mapSize) throw (hsds::Exception) {
    clear();
    alphabetNum_ = *(static_cast<uint64_t*>(ptr));
    uint64_t offset = sizeof(alphabetNum_);
    HSDS_EXCEPTION_IF(offset >= mapSize, E_LOAD_FILE);

    alphabetBitNum_ = log2(alphabetNum_);

    size_ = *(reinterpret_cast<uint64_t*>(reinterpret_cast<char*>(ptr) + offset));
    offset += sizeof(size_);
    HSDS_EXCEPTION_IF(offset >= mapSize, E_LOAD_FILE);

    bv_.resize(alphabetBitNum_);
    for (size_t i = 0; i < alphabetBitNum_; ++i) {
        offset += bv_[i].map(reinterpret_cast<char*>(ptr) + offset, mapSize - offset);
    }

    nodePos_.resize(bv_.size());
    for (size_t i = 0; i < bv_.size(); ++i) {
        offset += nodePos_[i].map(reinterpret_cast<char*>(ptr) + offset, mapSize - offset);
    }
    return offset;
}

uint64_t WaveletMatrix::prefixCode(uint64_t x, uint64_t len, uint64_t bit_num) const {
    return x >> (bit_num - len);
}

bool WaveletMatrix::checkPrefix(uint64_t prefix, uint64_t depth, uint64_t min_c, uint64_t max_c) const {
    if (prefixCode(min_c, depth, alphabetBitNum_) <= prefix && prefixCode(max_c - 1, depth, alphabetBitNum_) >= prefix)
        return true;
    else
        return false;
}

void WaveletMatrix::expandNode(uint64_t min_c, uint64_t max_c, const QueryOnNode& qon,
        vector<QueryOnNode>& next) const {
    const BitVector& bv = bv_[qon.depth];

    uint64_t beg_node_zero = bv.rank0(qon.beg_node);
    uint64_t end_node_zero = bv.rank0(qon.end_node);
    uint64_t beg_node_one = qon.beg_node - beg_node_zero;
    uint64_t beg_zero = bv.rank0(qon.beg_pos);
    uint64_t end_zero = bv.rank0(qon.end_pos);
    uint64_t beg_one = qon.beg_pos - beg_zero;
    uint64_t end_one = qon.end_pos - end_zero;
    uint64_t boundary = qon.beg_node + end_node_zero - beg_node_zero;
    if (end_zero - beg_zero > 0) { // child for zero
        uint64_t next_prefix = qon.prefix_char << 1;
        if (checkPrefix(next_prefix, qon.depth + 1, min_c, max_c)) {
            next.push_back(
                    QueryOnNode(qon.beg_node, boundary, qon.beg_node + beg_zero - beg_node_zero,
                            qon.beg_node + end_zero - beg_node_zero, qon.depth + 1, next_prefix));
        }
    }
    if (end_one - beg_one > 0) { // child for one
        uint64_t next_prefix = (qon.prefix_char << 1) + 1;
        if (checkPrefix(next_prefix, qon.depth + 1, min_c, max_c)) {
            next.push_back(
                    QueryOnNode(boundary, qon.end_node, boundary + beg_one - beg_node_one,
                            boundary + end_one - beg_node_one, qon.depth + 1, next_prefix));
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
