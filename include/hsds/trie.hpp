#if !defined(HSDS_TRIE_HPP_)
#define HSDS_TRIE_HPP_

#if !defined(_MSC_VER)
#include <stdint.h>
#endif // !defined(_MSC_VER)
#include <vector>
#include "hsds/vector.hpp"
#include "hsds/bit-vector.hpp"

namespace hsds {
class Trie {
public:
    typedef uint64_t id_t;
    static const id_t NOT_FOUND = ~(0ULL);
    static const id_t FAILED_TRAVERSE_AT_NODE = ~(0ULL) - 1;
    static const uint64_t DEFAULT_LIMIT_VALUE = ~(0ULL);

    Trie();

    virtual ~Trie();

    void build(std::vector<std::string>& keyList);

    id_t exactMatchSearch(const char* str, size_t len) const;

    void commonPrefixSearch(const char* str, 
                            size_t len, 
                            std::vector<id_t>& retIDs, 
                            uint64_t limit = DEFAULT_LIMIT_VALUE) const;

    void predictiveSearch(const char* str,
                          size_t len,
                          std::vector<id_t>& retIDs,
                           uint64_t limit = DEFAULT_LIMIT_VALUE) const;

    id_t traverse(const char* str, size_t len, uint64_t& nodePos, uint64_t& zeros, size_t& keyPos) const;

    void decodeKey(id_t id, std::string& key) const;

    void swap(Trie& other);

    void clear();
   
private:
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
    bool tailMatch(const char* str, size_t len, size_t depth,
            uint64_t tailID, size_t& retLen) const;
    std::string getTail(uint64_t i) const;
};

}

#endif
