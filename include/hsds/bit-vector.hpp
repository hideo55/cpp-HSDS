#if !defined(HSDS_BIT_VECTOR_H_)
#define HSDS_BIT_VECTOR_H_

#if !defined(_MSC_VER)
#include <stdint.h>
#endif // !defined(_MSC_VER)
#include <vector>
#include <iostream>
#include <algorithm>
#include <stdint.h>
#include "hsds/vector.hpp"
#include "hsds/rank-index.hpp"


/**
 * @namespace hsds
 */
namespace hsds {

namespace {
const uint32_t S_BLOCK_SIZE = 64;
const uint32_t L_BLOCK_SIZE = 512;
const uint32_t BLOCK_RATE = 8;

const char* const E_OUT_OF_RANGE = "Out of range access";
const char* const E_SAVE_FILE = "Failed to save the bit vector.";
const char* const E_LOAD_FILE = "Failed to read file. File format is invalid.";
}

class Exception;
class PopCount;

/**
 * @class BitVector
 * @brief Succinct bit vector class
 */
class BitVector {
public:
    /**
     * @brief Constructor
     */
    BitVector() :
            size_(0), num_of_1s_(0) {
    }

    /**
     * @brief Constructor
     *
     * @param[in] size Size of bit vector
     */
    BitVector(uint64_t size) :
            size_(size), num_of_1s_(0) {
        uint64_t block_num = (size + S_BLOCK_SIZE - 1) / S_BLOCK_SIZE;
        blocks_.resize(block_num);
    }

    /**
     * @brief Destructor
     */
    ~BitVector() {
    }

    /**
     * @brief Clear bit vector
     */
    void clear() {
        BitVector().swap(*this);
    }

    /**
     * @brief Get value from bit vector by index
     *
     * @param[in] i Index of bit vector
     *
     * @return The value of the specified index
     *
     * @exception hsds::Exception Out of range access
     */
    bool operator[](uint64_t i) const throw (hsds::Exception) {
        HSDS_DEBUG_IF(i >= size_, E_OUT_OF_RANGE);
        return (blocks_[i / S_BLOCK_SIZE] & (1ULL << (i % S_BLOCK_SIZE))) != 0;
    }

    /**
     * @brief Set value to bit vector by index
     *
     * @param[in] i Index of bit vector
     * @param[in] b Boolean value that indicates the bit to set.(true = 1, false = 0)
     */
    void set(uint64_t i, bool b = true);

    /**
     * @brief Build succinct bit vector
     *
     * @param[in] enable_faster_select1 Enable faster select1().
     * @param[in] enable_faster_select0 Enable faster select0().
     *
     */
    void build(bool enable_faster_select1 = false, bool enable_faster_select0 = false);

    /**
     * @brief Returns number of the element in bit vector
     *
     * @return Size of the bit vector
     */
    uint64_t size() const {
        return size_;
    }

    /**
     * @brief Returns the number of bits that matches with argument in the bit vector
     *
     * @param[in] b Boolean value that indicates bit type.(true = 1, false = 0)
     *
     * @return Number of bits that matches with argument in the bit vector
     */
    uint64_t size(bool b) const {
        return b ? (num_of_1s_) : (size_ - num_of_1s_);
    }

    /**
     * @brief Returns whether the vector is empty (i.e. whether its size is 0)
     *
     * @retval true Container size equals 0.
     * @retval false Container size not equals 0.
     */
    bool empty() const {
        return size_ == 0;
    }

    /**
     * @brief Returns Number of the bits equal to 0 up to position `i`
     *
     * @param[in] i Index of the bit vector
     *
     * @return Number of the bits equal to 0
     *
     * @exception hsds::Exception Out of range access
     */
    uint64_t rank0(uint64_t i) const throw (hsds::Exception);

    /**
     * @brief Returns Number of the bits equal to 1 up to position `i`
     *
     * @param[in] i Index of the bit vector
     *
     * @return Number of the bits equal to 1
     *
     * @exception hsds::Exception Out of range access
     */
    uint64_t rank1(uint64_t i) const throw (hsds::Exception);

    /**
     * @brief Returns the position of the x-th occurrence of 0
     *
     * @param[in] x Rank number of 0-bits
     *
     * @return Index of x-th 0
     *
     * @exception hsds::Exception Out of range access
     */
    uint64_t select0(uint64_t x) const throw (hsds::Exception);

    /**
     * @brief Returns the position of the x-th occurrence of 1
     *
     * @param[in] x Rank number of 1-bits
     *
     * @return Index of x-th 1
     *
     * @exception hsds::Exception Out of range access
     */
    uint64_t select1(uint64_t x) const throw (hsds::Exception);

    /**
     * @brief Save bit vector to the ostream
     *
     * @param[out] os The instance of std::ostream
     */
    void save(std::ostream &os) const;

    /**
     * @brief Load bit vector from istream
     *
     * @param[in] is The instance of std::istream
     */
    void load(std::istream &is);

    /**
     * @brief Mapping pointer to bit vector
     *
     * @param[in] ptr Pointer of the mmaped file
     * @param[in] size Size of mmaped file
     */
    void map(void* ptr, uint64_t size);

    /**
     * @brief Exchanges the content of the instance
     *
     * @param[in,out] x Another BitVector instnace
     */
    void swap(BitVector &x) {
        blocks_.swap(x.blocks_);
        std::swap(size_, x.size_);
        std::swap(num_of_1s_, x.num_of_1s_);
        rank_table_.swap(x.rank_table_);
        select0_table_.swap(x.select0_table_);
        select1_table_.swap(x.select1_table_);
    }

private:
    typedef uint64_t block_type;
    typedef hsds::Vector<block_type> blocks_type;
    typedef hsds::Vector<RankIndex> rank_dict_type;
    typedef hsds::Vector<uint32_t> select_dict_type;

    blocks_type blocks_;                ///< Bit vector
    rank_dict_type rank_table_;         ///< Rank dictionary
    select_dict_type select0_table_;    ///< Select dictionary for 0-bits
    select_dict_type select1_table_;    ///< Select dictionary for 1-bits
    uint64_t size_;                     ///< Size of bit vector
    uint64_t num_of_1s_;                ///< Nuber of the 1-bits

    // Disable copy constructor
    BitVector(const BitVector &);
    // Disable assingment operator
    BitVector &operator=(const BitVector &);
};

}

#endif /* !defined(HSDS_BIT_VECTOR_H_) */
