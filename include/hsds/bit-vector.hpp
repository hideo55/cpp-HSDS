#ifndef HSDS_BIT_VECTOR_H_
#define HSDS_BIT_VECTOR_H_

#include <stdint.h>
#include <vector>
#include <iostream>
#include "hsds/popcount.hpp"
#include "hsds/rank-index.hpp"


namespace hsds {

const uint32_t S_BLOCK_SIZE = 64;
const uint32_t L_BLOCK_SIZE = 512;
const uint32_t BLOCK_RATE = 8;

class BitVector {
public:
    BitVector();
    ~BitVector();
    BitVector(uint64_t size);

    void clear();
    bool get(uint64_t i) const;
    void set(uint64_t i, bool b = true);

    void build();

    uint64_t size() const {
        return size_;
    }
    uint64_t size(bool b) const {
        return b ? (num_of_1s_) : (size_ - num_of_1s_);
    }

    uint64_t rank(uint64_t i) const;
    uint64_t select0(uint64_t i) const;
    uint64_t select1(uint64_t i) const;

    void save(std::ostream &os) const;
    void load(std::istream &is);

private:
    typedef uint64_t block_type;
    typedef std::vector<block_type> blocks_type;
    typedef std::vector<RankIndex> ranktable_type;
    typedef std::vector<uint32_t> select_dict_type;

    blocks_type blocks_;
    ranktable_type rank_tables_;
    select_dict_type select_0s_;
    select_dict_type select_1s_;
    uint64_t size_;
    uint64_t num_of_1s_;
};

}

#endif /* HSDS_BIT_VECTOR_H_ */
