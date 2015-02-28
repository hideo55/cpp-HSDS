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
    typedef uint64_t id_t; ///< ID of leaf-node
    static const id_t NOT_FOUND = ~(0ULL); ///< data not found
    static const id_t CAN_NOT_TRAVERSE = ~(0ULL) - 1; ///< Can't traverse the next node

    typedef struct Result {
        id_t id; ///< ID of leaf-node
        uint64_t depth; ///< depth of node

        Result(id_t id_, uint64_t depth_) :
                id(id_), depth(depth_) {
        }
    } Result;

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
     * @param[in] useTailTrie use tail compression
     */
    void build(std::vector<std::string>& keyList, bool useTailTrie = false);

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
     * Searches keys from the possible prefixes of a query string
     *
     * @param[in] str query string
     * @param[in] len length of str
     * @param[out] results search results
     * @param[in] limit the upper limit of the ID number to retrieve
     */
    void commonPrefixSearch(const char* str, size_t len, std::vector<Result>& results, uint64_t limit =
            DEFAULT_LIMIT_VALUE) const;

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
     * @param[in] str query string
     * @param[in] len length of str
     * @param[in,out] nodePos current position in trie
     * @param[in,out] zeros 
     * @param[in,out] keyPos current position in key
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

    /**
     * Return the number of keys in the dictionary
     * @return the number of keys in the dictionary
     */
    size_t size() const;

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
     * @brief Mapping pointer to the Trie
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
    static const uint64_t DEFAULT_LIMIT_VALUE = ~(0ULL); ///< Default value of the limit
    BitVector louds_;
    BitVector terminal_;
    BitVector tail_;

    Vector<Vector<char> > vtails_;
    Vector<uint8_t> edges_;
    size_t numOfKeys_;
    bool isReady_;

    Trie* vtailTrie_;
    BitVector tailIDs_;
    uint64_t tailIDSize_;

    void build(hsds::Vector<Vector<char>  >& keyList);// for build tail trie
    void buildTailTrie();
    bool isLeaf(uint64_t pos) const;
    void getChild(uint8_t c, uint64_t& pos, uint64_t& zeros) const;
    void getParent(uint8_t& c, uint64_t& pos, uint64_t& zeros) const;
    void enumerateAll(uint64_t pos, uint64_t zeros, std::vector<id_t>& retIDs, size_t limit) const;
    bool tailMatch(const char* str, size_t len, size_t depth, uint64_t tailID, size_t& retLen) const;
    Vector<char> getTail(uint64_t i) const;
};

}

#endif
