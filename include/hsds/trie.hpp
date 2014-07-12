/**
 * @file trie.hpp
 * @brief Definition of Trie
 * @author Hideaki Ohno
 */
#if !defined(HSDS_TRIE_HPP_)
#define HSDS_TRIE_HPP_

#include <vector>
#include "hsds/vector.hpp"
#include "hsds/bit-vector.hpp"

/**
 * @brief The namespace for Hide's Succinct Data Structures
 * @namespace hsds
 */
namespace hsds {

/**
 * @brief Trie(LOUDS) class
 */
class Trie {
public:
    typedef uint64_t id_t;                              ///< ID of leaf-node
    static const id_t NOT_FOUND = ~(0ULL);              ///< data not found
    static const id_t CAN_NOT_TRAVERSE = ~(0ULL) - 1;   ///< Can't traverse the next node


    /**
     * Constructor
     */
    Trie();

    /**
     * Destructor
     */
    virtual ~Trie();

    /**
     * Build louds trie
     *
     * @param[in] keyList source data of trie
     */
    void build(std::vector<std::string>& keyList);

    /**
     * Searches key exact match with query string.
     *
     * @param[in] str query string
     * @param[in] len length of str
     *
     * @retval Trie::NOT_FOUND Key does not found in the trie
     * @return ID of the key.
     */
    id_t exactMatchSearch(const char* str, size_t len) const;

    /**
     * Searches keys from the possible prefixes of a query string
     *
     * @param[in] str query string
     * @param[in] len length of str
     * @param[out] retIDs IDs of keys
     * @param[in] limit the upper limit of the ID number to retrieve
     */
    void commonPrefixSearch(const char* str, size_t len, std::vector<id_t>& retIDs,
            uint64_t limit = DEFAULT_LIMIT_VALUE) const;

    /**
     * Searches keys starting with a query string
     *
     * @param[in] str query string
     * @param[in] len length of str
     * @param[out] retIDs IDs of keys
     * @param[in] limit the upper limit of the ID number to retrieve
     */
    void predictiveSearch(const char* str, size_t len, std::vector<id_t>& retIDs,
            uint64_t limit = DEFAULT_LIMIT_VALUE) const;

    /**
     * Traverse the node of the trie
     *
     * @retval Trie::CAN_NOT_TRAVERSE
     * @retval Trie::NOT_FOUND
     * @return ID of the leaf-node
     */
    id_t traverse(const char* str, size_t len, uint64_t& nodePos, uint64_t& zeros, size_t& keyPos) const;

    /**
     * Get key string corresponding to the ID
     *
     * @param[in] id ID of key.
     * @param[out] key
     */
    void decodeKey(id_t id, std::string& key) const;

    /**
     * Exchanges the content of the instance
     *
     * @param[in,out] x Another Trie instance
     */
    void swap(Trie& x);

    /**
     * Clear trie
     */
    void clear();

private:
    static const uint64_t DEFAULT_LIMIT_VALUE = ~(0ULL);///< Default value of the limit
    BitVector louds_;
    BitVector terminal_;
    BitVector tail_;

    Vector<std::string> vtails_;
    Vector<uint8_t> edges_;
    size_t numOfKeys_;
    bool isReady_;

    bool isLeaf(uint64_t pos) const;
    void getChild(uint8_t c, uint64_t& pos, uint64_t& zeros) const;
    void getParent(uint8_t& c, uint64_t& pos, uint64_t& zeros) const;
    void enumerateAll(uint64_t pos, uint64_t zeros, std::vector<id_t>& retIDs, size_t limit) const;
    bool tailMatch(const char* str, size_t len, size_t depth, uint64_t tailID, size_t& retLen) const;
    std::string getTail(uint64_t i) const;
};

}

#endif
