#ifndef HSDS_BIT_VECTOR_H_
#define HSDS_BIT_VECTOR_H_

#include <stdint.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdint.h>
#include "hsds/exception.hpp"
#include "hsds/popcount.hpp"
#include "hsds/rank-index.hpp"

namespace hsds {

const uint32_t S_BLOCK_SIZE = 64;
const uint32_t L_BLOCK_SIZE = 512;
const uint32_t BLOCK_RATE = 8;

/**
 * @class BitVector
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
     * @return The value of the specified index
     * @exception hsds::Exception Out of range access
     */
    bool operator[](uint64_t i) const throw (hsds::Exception) {
        HSDS_EXCEPTION_IF(i >= size_, "Out of range access");
        return (blocks_[i / S_BLOCK_SIZE] & (1ULL << (i % S_BLOCK_SIZE))) != 0;
    }

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
     * @brief Returns number of the element in bit vector
     * @return Size of the bit vector
     */
    uint64_t size() const {
        return size_;
    }
    /**
     * @brief Returns the number of bits that matches with argument in the bit vector.
     * @param b[in] Boolean
     * @return Number of bits that matches with argument in the bit vector.
     */
    uint64_t size(bool b) const {
        return b ? (num_of_1s_) : (size_ - num_of_1s_);
    }

    /**
     * @brief Returns whether the vector is empty (i.e. whether its size is 0).
     * @return true if the container size is 0, false otherwise.
     */
    bool empty() const {
        return size_ == 0;
    }

    /**
     * @brief Returns Number of the bits equal to 1 up to position `i`.
     * @param i[in] Index of the bit vector
     * @return Number of the bits equal to 1
     * @exception hsds::Exception Out of range access
     */
    uint64_t rank(uint64_t i) const throw (hsds::Exception);

    /**
     * @brief Returns the position of the x-th occurrence of 0.
     * @param x[in] Rank number of 0-bits
     * @return Index of x-th 0.
     * @exception hsds::Exception Out of range access
     */
    uint64_t select0(uint64_t x) const throw (hsds::Exception);

    /**
     * @brief Returns the position of the x-th occurrence of 1.
     * @param x[in] Rank number of 1-bits
     * @return Index of x-th 1.
     * @exception hsds::Exception Out of range access
     */
    uint64_t select1(uint64_t x) const throw (hsds::Exception);

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

    /**
     * @brief Exchanges the content of the instance.
     * @param x[in,out] Another BitVector instnace
     */
    void swap(BitVector &x) {
        blocks_.swap(x.blocks_);
        std::swap(size_, x.size_);
        std::swap(num_of_1s_, x.num_of_1s_);
        rank_tables_.swap(x.rank_tables_);
        select_0s_.swap(x.select_0s_);
        select_1s_.swap(x.select_1s_);
    }
private:
    typedef uint64_t block_type;
    typedef std::vector<block_type> blocks_type;
    typedef std::vector<RankIndex> ranktable_type;
    typedef std::vector<uint32_t> select_dict_type;

    blocks_type blocks_;            ///< Bit vector
    ranktable_type rank_tables_;    ///< Rank dictionary
    select_dict_type select_0s_;    ///< Select dictionary for 0-bits
    select_dict_type select_1s_;    ///< Select dictionary for 1-bits
    uint64_t size_;                 ///< Size of bit vector
    uint64_t num_of_1s_;            ///< Nuber of the 1-bits

    // Disable copy constructor
    BitVector(const BitVector &);
    // Disable assingment operator
    BitVector &operator=(const BitVector &);
};

}

#endif /* HSDS_BIT_VECTOR_H_ */
