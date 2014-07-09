/**
 * @file wavelet-matrix.hpp
 * @brief Definition of WaveletMatrix
 * @author Hideaki Ohno
 */

#if !defined(HSDS_WAVELET_MATRIX_HPP_)
#define HSDS_WAVELET_MATRIX_HPP_

#include <vector>
#include <queue>
#include "hsds/scoped_ptr.hpp"
#include "hsds/bit-vector.hpp"
#include "hsds/vector.hpp"

/**
 * @brief The namespace for Hide's Succinct Data Structures
 * @namespace hsds
 */
namespace hsds {

/**
 * @brief Return a value of specified position in bit sequence.
 *
 * @param[in] bits bit sequence(unsigned integer)
 * @param[in] pos Position of bit.
 *
 * @return boolean value
 */
bool uint2bit(uint64_t bits, uint64_t pos) {
    return ((bits >> (sizeof(uint64_t) * 8 - 1 - pos)) & 0x1ULL) == 0x1ULL;
}

// forward declaration
class Exception;

/**
 * @brief Result of list* API
 */
struct ListResult {
    /**
     * Constructor
     *
     * @param[in] c The character
     * @param[in] freq The frequency of c in the array
     */
    ListResult(uint64_t c, uint64_t freq) :
            c(c), freq(freq) {
    }
    uint64_t c;     ///< The character
    uint64_t freq;  ///< The frequency of c in the array

    /**
     * Compare operator
     * @param[in] lr ListResult object
     */
    bool operator <(const ListResult& lr) const {
        if (c != lr.c)
            return c < lr.c;
        return freq < lr.freq;
    }
};

/**
 * @brief Wavelet matrix class. See also http://www.dcc.uchile.cl/~gnavarro/ps/spire12.4.pdf
 */
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

    /**
     * Clear wavelet matrix
     */
    void clear();

    /**
     * Exchanges the content of the instance
     *
     * @param[in,out] x Another WaveletMatrix instance
     */
    void swap(WaveletMatrix& x);

    /**
     * Build the wavelet matrix form array(`src`)
     *
     * @param[in] src An array to be initialized
     */
    void build(std::vector<uint64_t>& src);

    /**
     *  Return the number of elements.
     *
     *  @return number of elements.
     */
    inline uint64_t size() const {
        return size_;
    }

    /**
     * Lookup A[pos]
     *
     * @param[in] pos The position
     *
     * @return return A[pos] if found, or return NOT_FOUND if pos >= size
     */
    inline uint64_t operator[](uint64_t pos) {
        return lookup(pos);
    }

    /**
     * Lookup A[pos]
     * 
     * @param[in] pos The position
     *
     * @return return A[pos] if found, or return NOT_FOUND if pos >= size
     */
    uint64_t lookup(uint64_t pos) const;

    /**
     * Compute the rank = the frequency of a character 'c' in the prefix of the array A[0...pos)
     * 
     * @param[in] c Character to be examined
     * @param[in] pos The position of the prefix (not inclusive)
     *
     * @return The frequency of a character 'c' in the prefix of the array A[0...pos)
     *         or NOT_FOUND if c >= alphabet_num or pos > size
     */
    uint64_t rank(uint64_t c, uint64_t pos) const;

    /**
     * Compute the frequency of characters c' < c in the subarray A[0...pos) 
     *
     * @param[in] c The upper bound of the characters
     * @param[in] pos The position of the end of the prefix (not inclusive)
     *
     * @return The frequency of characters c' < c in the prefix of the array A[0...pos)
     *              or NOTFOUND if c > alphabet_num or pos > size
     */
    uint64_t rankLessThan(uint64_t c, uint64_t pos) const;

    /**
     * Compute the frequency of characters c' > c in the subarray A[0...pos)
     *
     * @param[in] c The lower bound of the characters
     * @param[in] pos The position of the end of the prefix (not inclusive)
     *
     * @return The frequency of characters c' < c in the prefix of the array A[0...pos)
     *         or NOTFOUND if c > alphabet_num or pos > length
     */
    uint64_t rankMoreThan(uint64_t c, uint64_t pos) const;

    /**
     * Compute the frequency of characters c' < c, c'=c, and c' > c, in the subarray A[begin_pos...end_pos)
     * @param c The character
     * @param begin_pos The beginning position of the array (not inclusive)
     * @param end_pos The ending position of the array (not inclusive)
     * @param rank The frefquency of c in A[0...pos)
     * @param rank_less_than The frequency of c' < c in A[0...pos)
     * @param rank_more_than The frequency of c' > c in A[0...pos)
     */
    void rankAll(uint64_t c, uint64_t begin_pos, uint64_t end_pos, uint64_t& rank, uint64_t& rank_less_than,
            uint64_t& rank_more_than) const;

    /**
     * Compute the select = the position of the (rank+1)-th occurrence of 'c' in the array.
     * 
     * @param[in] c Character to be examined
     * @param[in] rank The rank of the character
     *
     * @return The position of the (rank+1)-th occurrence of 'c' in the array.
     *         or NOT_FOUND if c >= alphabet_num or rank > freq(c)
     */
    uint64_t select(uint64_t c, uint64_t rank) const;

    /**
     * Compute the select = the position of the (rank+1)-th occurrence of 'c' in the suffix of the array starting from 'pos'
     *
     * @param[in] c Character to be examined
     * @param[in] pos The beginning position of the suffix (inclusive)
     * @param[in] rank The rank of the character
     * @return The position of the (rank+1)-th occurrence of 'c' in the suffix of the array.
     *         or NOT_FOUND if c >= alphabet_num or rank > freq(c) - rank(c, pos)
     */
    uint64_t selectFromPos(uint64_t c, uint64_t pos, uint64_t rank) const;

    /**
     * Compute the frequency of the character c
     *
     * @param[in] c The character to be examined
     *
     * @return Return the frequency of c in the array.
     */
    uint64_t freq(uint64_t c) const;

    /**
     * Compute the frequency of the characters
     *
     * @param[in] min_c The minimum character
     * @param[in] max_c The maximum character
     *
     * @return  Return the frequency of min_c <= c < max_c in the array.
     */
    uint64_t freqSum(uint64_t min_c, uint64_t max_c) const;

    /**
     * Compute the frequency of characters min_c <= c' < max_c in the subarray A[beg_pos ... end_pos)
     *
     * @param[in] min_c The smallest character to be examined
     * @param[in] max_c The upper bound of the character to be examined
     * @param[in] begin_pos The beginning position of the array (inclusive)
     * @param[in] end_pos The ending position of the array (not inclusive)
     *
     * @return The frequency of characters min_c <= c < max_c in the subarray A[beg_pos .. end_pos)
     *          or NOTFOUND if max_c > alphabet_num or end_pos > length
     */
    uint64_t freqRange(uint64_t min_c, uint64_t max_c, uint64_t begin_pos, uint64_t end_pos) const;

    /**
     * Range Max Query
     *
     * @param[in] begin_pos The beginning position
     * @param[in] end_pos The ending position
     * @param[out] pos The position where the largest value appeared in the subarray A[beg_pos .. end_pos)
     *            If there are many items having the largest values, the smallest pos will be reported
     * @param[out] val The largest value appeared in the subarray A[beg_pos ... end_pos)
     */
    void maxRange(uint64_t begin_pos, uint64_t end_pos, uint64_t& pos, uint64_t& val) const;

    /**
     * Range Min Query
     *
     * @param[in] begin_pos The beginning position
     * @param[in] end_pos The ending position
     * @param[out] pos The position where the smallest value appeared in the subarray A[beg_pos .. end_pos)
     *            If there are many items having the smallest values, the smallest pos will be reported
     * @param[out] val The smallest value appeared in the subarray A[beg_pos ... end_pos)
     */
    void minRange(uint64_t begin_pos, uint64_t end_pos, uint64_t& pos, uint64_t& val) const;

    /**
     * Range Quantile Query, Return the K-th smallest value in the subarray
     *
     * @param[in] begin_pos The beginning position
     * @param[in] end_pos The ending position
     * @param[in] k The order (should be smaller than end_pos - beg_pos).
     * @param[out] pos The position where the k-th largest value appeared in the subarray A[beg_pos .. end_pos)
     *            If there are many items having the k-th largest values, the smallest pos will be reported
     * @param[out] val The k-th largest value appeared in the subarray A[beg_pos ... end_pos)
     */
    void quantileRange(uint64_t begin_pos, uint64_t end_pos, uint64_t k, uint64_t& pos, uint64_t& val) const;

    /**
     * List the distinct characters appeared in A[beg_pos ... end_pos) from most frequent ones
     *
     * @param min_c The smallest character to be examined
     * @param max_c The upper bound of the character to be examined
     * @param beg_pos The beginning position of the array (inclusive)
     * @param end_pos The ending position of the array (not inclusive)
     * @param num The maximum number of reporting results.
     * @param res The distinct characters in the A[beg_pos ... end_pos) from most frequent ones.
     *            Each item consists of c:character and freq: frequency of c.
     */
    void listModeRange(uint64_t min_c, uint64_t max_c, uint64_t beg_pos, uint64_t end_pos, uint64_t num,
            std::vector<ListResult>& res) const;

    /**
     * List the distinct characters in A[beg_pos ... end_pos) min_c <= c < max_c  from smallest ones
     *
     * @param min_c The smallest character to be examined
     * @param max_c The upper bound of the character to be examined
     * @param beg_pos The beginning position of the array (inclusive)
     * @param end_pos The ending position of the array (not inclusive)
     * @param num The maximum number of reporting results.
     * @param res The distinct characters in the A[beg_pos ... end_pos) from smallest ones.
     *            Each item consists of c:character and freq: frequency of c.
     */
    void listMinRange(uint64_t min_c, uint64_t max_c, uint64_t beg_pos, uint64_t end_pos, uint64_t num,
            std::vector<ListResult>& res) const;

    /**
     * List the distinct characters appeared in A[beg_pos ... end_pos) from largest ones
     *
     * @param min_c The smallest character to be examined
     * @param max_c The upper bound of the character to be examined
     * @param beg_pos The beginning position of the array (inclusive)
     * @param end_pos The ending position of the array (not inclusive)
     * @param num The maximum number of reporting results.
     * @param res The distinct characters in the A[beg_pos ... end_pos) from largest ones.
     *            Each item consists of c:character and freq: frequency of c.
     */
    void listMaxRange(uint64_t min_c, uint64_t max_c, uint64_t beg_pos, uint64_t end_pos, uint64_t num,
            std::vector<ListResult>& res) const;

    /**
     * Save the current status to a stream
     *
     * @param[out] os The output stream where the data is saved
     *
     * @exception hsds::Exception When failed to save.
     */
    void save(std::ostream& os) const throw (hsds::Exception);

    /**
     * Load the current status from a stream
     *
     * @param[in] is The input stream where the status is saved
     *
     * @exception hsds::Exception When failed to load.
     */
    void load(std::istream& is) throw (hsds::Exception);

    /**
     * @brief Mapping pointer to the WaveletMatrix
     *
     * @param[in] ptr The pointer of the mmaped file
     * @param[in] mapSize The size of mmaped file
     *
     * @return Actually mapped size(byte size of offset from `ptr`).
     *
     * @exception hsds::Exception When failed to load.
     */
    uint64_t map(void* ptr, uint64_t mapSize) throw (hsds::Exception);

private:
    typedef hsds::Vector<hsds::BitVector> bv_type;
    typedef hsds::Vector<hsds::Vector<uint64_t> > range_type;
    typedef hsds::Vector<uint64_t> uint64_vector_type;

    uint64_t size_;
    const uint64_t bitSize_;
    uint64_t alphabetNum_;
    uint64_t alphabetBitNum_;
    bv_type bv_;
    range_type nodePos_;
    uint64_vector_type seps_;

    inline uint64_t bitSize() const {
        return bitSize_;
    }

    uint64_t getAlphabetNum(const std::vector<uint64_t>& array) const;
    uint64_t log2(uint64_t x) const;

    struct QueryOnNode {
        QueryOnNode(uint64_t beg_node, uint64_t end_node, uint64_t beg_pos, uint64_t end_pos, uint64_t depth,
                uint64_t prefix_char) :
                beg_node(beg_node),
                    end_node(end_node),
                    beg_pos(beg_pos),
                    end_pos(end_pos),
                    depth(depth),
                    prefix_char(prefix_char) {
        }
        uint64_t beg_node;
        uint64_t end_node;
        uint64_t beg_pos;
        uint64_t end_pos;
        uint64_t depth;
        uint64_t prefix_char;
        void print() {
            std::cout << beg_node << " " << end_node << " " << beg_pos << " " << end_pos << " " << depth << " ";
            for (uint64_t i = 0; i < depth; ++i) {
                std::cout << ((prefix_char >> (depth - (i + 1))) & 1LLU);
            }
            std::cout << std::endl;
        }
    };

    class ListModeComparator;
    class ListMinComparator;
    class ListMaxComparator;

    template<class Comparator>
    void listRange(uint64_t min_c, uint64_t max_c, uint64_t beg_pos, uint64_t end_pos, uint64_t num,
            std::vector<ListResult>& res) const {
        res.clear();
        if (end_pos > size_ || beg_pos >= end_pos)
            return;

        std::priority_queue<QueryOnNode, std::vector<QueryOnNode>, Comparator> qons;
        qons.push(QueryOnNode(0, size_, beg_pos, end_pos, 0, 0));

        while (res.size() < num && !qons.empty()) {
            QueryOnNode qon = qons.top();
            qons.pop();
            if (qon.depth >= alphabetBitNum_) {
                res.push_back(ListResult(qon.prefix_char, qon.end_pos - qon.beg_pos));
            } else {
                std::vector<QueryOnNode> next;
                expandNode(min_c, max_c, qon, next);
                for (size_t i = 0; i < next.size(); ++i) {
                    qons.push(next[i]);
                }
            }
        }
    }

    uint64_t prefixCode(uint64_t x, uint64_t len, uint64_t bit_num) const;
    bool checkPrefix(uint64_t prefix, uint64_t depth, uint64_t min_c, uint64_t max_c) const;
    void expandNode(uint64_t min_c, uint64_t max_c, const QueryOnNode& qon, std::vector<QueryOnNode>& next) const;
};

}

#endif /* !defined(HSDS_WAVELET_MATRIX_HPP_) */
