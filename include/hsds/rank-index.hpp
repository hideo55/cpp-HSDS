/**
 * @file rank-index.hpp
 * @brief Implementation of RankIndex class
 * @author Hideaki Ohno
 */

#if !defined(HSDS_RANK_INDEX_H_)
#define HSDS_RANK_INDEX_H_

#if !defined(_MSC_VER)
#include <stdint.h>
#endif // !defined(_MSC_VER)

/**
 * @namespace hsds
 */
namespace hsds {

/**
 * @class RankIndex
 *
 * @brief Index structure for rank operation.
 */
class RankIndex {
public:
    /**
     * @brief Constructor
     */
    RankIndex() :
            abs_(0), rel_(0) {
    }

    /**
     * @brief Destructor
     */
    ~RankIndex() {

    }

    /**
     * @brief Setter method for absolute rank value.
     *
     * @param[in] value Absolute value of rank.
     */
    void set_abs(uint32_t value) {
        abs_ = value;
    }

    /**
     * @brief Setter method for relative rank value at 1st block.
     *
     * @param[in] value Relative rank value.
     */
    void set_rel1(uint64_t value) {
        rel_ = ((rel_ & ~0x7FULL) | (value & 0x7FULL));
    }

    /**
     * @brief Setter method for relative rank value at 2nd block.
     *
     * @param[in] value Relative rank value.
     */
    void set_rel2(uint64_t value) {
        rel_ = ((rel_ & ~(0xFFULL << 7)) | (value & 0xFFULL) << 7);
    }

    /**
     * @brief Setter method for relative rank value at 3rd block.
     *
     * @param[in] value Relative rank value.
     */
    void set_rel3(uint64_t value) {
        rel_ = ((rel_ & ~(0xFFULL << 15)) | (value & 0xFFULL) << 15);
    }

    /**
     * @brief Setter method for relative rank value at 4th block.
     *
     * @param[in] value Relative rank value.
     */
    void set_rel4(uint64_t value) {
        rel_ = ((rel_ & ~(0x1FFULL << 23)) | (value & 0x1FFULL) << 23);
    }

    /**
     * @brief Setter method for relative rank value at 5th block.
     *
     * @param[in] value Relative rank value.
     */
    void set_rel5(uint64_t value) {
        rel_ = ((rel_ & ~(0x1FFULL << 32)) | (value & 0x1FFULL) << 32);
    }

    /**
     * @brief Setter method for relative rank value at 6th block.
     *
     * @param[in] value Relative rank value.
     */
    void set_rel6(uint64_t value) {
        rel_ = ((rel_ & ~(0x1FFULL << 41)) | (value & 0x1FFULL) << 41);
    }

    /**
     * @brief Setter method for relative rank value at 7th block.
     *
     * @param[in] value Relative rank value.
     */
    void set_rel7(uint64_t value) {
        rel_ = ((rel_ & ~(0x1FFULL << 50)) | (value & 0x1FFULL) << 50);
    }

    /**
     * @brief Getter method for absolute rank value.
     *
     * @return Absolute rank value.
     */
    uint64_t abs() const {
        return abs_;
    }

    /**
     * @brief Getter method for relative value at 1st block.
     *
     * @return Relative value at 1st block.
     */
    uint64_t rel1() const {
        return rel_ & 0x7FULL;
    }

    /**
     * @brief Getter method for relative value at 2nd block.
     *
     * @return Relative value at 1st block.
     */
    uint64_t rel2() const {
        return (rel_ >> 7) & 0xFFULL;
    }

    /**
     * @brief Getter method for relative value at 3rd block.
     *
     * @return Relative value at 1st block.
     */
    uint64_t rel3() const {
        return (rel_ >> 15) & 0xFFULL;
    }

    /**
     * @brief Getter method for relative value at 4th block.
     *
     * @return Relative value at 1st block.
     */
    uint64_t rel4() const {
        return (rel_ >> 23) & 0x1FFULL;
    }

    /**
     * @brief Getter method for relative value at 5th block.
     *
     * @return Relative value at 1st block.
     */
    uint64_t rel5() const {
        return (rel_ >> 32) & 0x1FFULL;
    }

    /**
     * @brief Getter method for relative value at 6th block.
     *
     * @return Relative value at 1st block.
     */
    uint64_t rel6() const {
        return (rel_ >> 41) & 0x1FFULL;
    }

    /**
     * @brief Getter method for relative value at 7th block.
     *
     * @return Relative value at 1st block.
     */
    uint64_t rel7() const {
        return (rel_ >> 50) & 0x1FFULL;
    }

private:
    uint64_t abs_;  ///< Absolute rank value
    uint64_t rel_;  ///< Relative rank value container
};

}

#endif /* !defined(HSDS_RANK_INDEX_H_) */
