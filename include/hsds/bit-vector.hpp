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

/**
 * @brief Succinct bit vector class
 */
class BitVector {
public:
    /**
     * @brief Constructor
     */
    BitVector();

    /**
     * @brief Constructor
     * @param Size[in] size of bit vector
     */
    BitVector(uint64_t size);

    /**
     * @brief Destructor
     */
    ~BitVector();

    /**
     * @brief Clear bit vector
     */
    void clear();

    /**
     * @brief Get value from bit vector by index
     * @param i[in] Index of bit vector
     * @retval The value of the specified index
     */
    bool get(uint64_t i) const;

    /**
     * @brief Set value to bit vector by index
     * @param i[in] Index of bit vector
     */
    void set(uint64_t i, bool b = true);

    /**
     * @brief Build succinct bit vector
     */
    void build();

    /**
     * @brief Returns size of the bit vector
     * @retval Size of the bit vector
     */
    uint64_t size() const {
        return size_;
    }
    /**
     * @brief Returns number of 1 or 0 in the bit vector
     * @param b[in]
     * @retval Nump
     */
    uint64_t size(bool b) const {
        return b ? (num_of_1s_) : (size_ - num_of_1s_);
    }

    /**
     * @brief
     * @param
     * @retval
     */
    uint64_t rank(uint64_t i) const;

    /**
     * @brief
     * @param
     * @retval
     */
    uint64_t select0(uint64_t i) const;

    /**
     * @brief
     * @param
     * @retval
     */
    uint64_t select1(uint64_t i) const;

    /**
     * @brief Save bit vector to the ostream
     * @param os[out] The instance of std::ostream
     */
    void save(std::ostream &os) const;

    /**
     * @brief Load bit vector from istream.
     * @param is[in] The instance of std::istream
     */
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
