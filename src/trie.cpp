#include "hsds/trie.hpp"
#include <algorithm>
#include <queue>
#include <cmath>
#include <cstring>

namespace hsds {

using namespace std;

struct RangeNode {
    RangeNode(size_t _left, size_t _right) :
            left(_left), right(_right) {
    }
    size_t left;
    size_t right;
};

FORCE_INLINE uint64_t lg2(const uint64_t x){
  uint64_t ret = 0;
  while (x >> ret){
    ++ret;
  }
  return ret;
}

Trie::Trie() :
        louds_(),
        terminal_(),
        tail_(),
        vtails_(),
        edges_(),
        numOfKeys_(0),
        isReady_(false),
        vtailTrie_(NULL),
        tailIDs_(),
        tailIDSize_(0) {

}

Trie::~Trie() {
}

void Trie::build(vector<string>& keyList, bool useTailTrie) {
    clear();
    sort(keyList.begin(), keyList.end());
    keyList.erase(unique(keyList.begin(), keyList.end()), keyList.end());

    numOfKeys_ = keyList.size();

    queue<RangeNode> q;
    queue<RangeNode> nextQ;

    if (numOfKeys_ != 0) {
        q.push(RangeNode(0, numOfKeys_));
    }

    BitVector loudBV, terminalBV, tailBV;
    loudBV.push_back(false);
    loudBV.push_back(true);

    for (size_t depth = 0;;) {
        if (q.empty()) {
            std::swap(q, nextQ);
            ++depth;
            if (q.empty()) {
                break;
            }
        }
        RangeNode& rn = q.front();
        const size_t left = rn.left;
        const size_t right = rn.right;
        q.pop();
        string& cur = keyList[left];
        if (left + 1 == right && depth + 1 < cur.size()) { // tail candidate
            loudBV.push_back(true);
            terminalBV.push_back(true);
            tailBV.push_back(true);
            Vector<char> tail;
            for (size_t i = depth; i < cur.size(); ++i) {
                tail.push_back(cur[i]);
            }
            vtails_.push_back(tail);
            continue;
        } else {
            tailBV.push_back(false);
        }

        size_t newLeft = left;
        if (depth == cur.size()) {
            terminalBV.push_back(true);
            ++newLeft;
            if (newLeft == right) {
                loudBV.push_back(true);
                continue;
            }
        } else {
            terminalBV.push_back(false);
        }

        size_t prev = newLeft;
        uint8_t prevC = (uint8_t) keyList[prev][depth];
        for (size_t i = prev + 1;; ++i) {
            if (i < right && prevC == (uint8_t) keyList[i][depth]) {
                continue;
            }
            edges_.push_back(prevC);
            loudBV.push_back(false);
            nextQ.push(RangeNode(prev, i));
            if (i == right) {
                break;
            }
            prev = i;
            prevC = keyList[prev][depth];
        }
        loudBV.push_back(true);

    }

    loudBV.build(true, true);
    louds_.swap(loudBV);
    terminalBV.build(true);
    terminal_.swap(terminalBV);
    tailBV.build();
    tail_.swap(tailBV);

    if (numOfKeys_ > 0) {
        isReady_ = true;
    }

    if (useTailTrie) {
        buildTailTrie();
    }
}

void Trie::build(Vector<Vector<char> >& keyList) {
    size_t keyNum = keyList.size();
    std::vector<string> newList;
    newList.reserve(keyNum);
    for (size_t i = 0; i < keyNum; ++i) {
        newList.push_back(string(&keyList[i][0], keyList[i].size()));
    }
    Vector<Vector<char> >().swap(keyList);
    build(newList, false);
}

void Trie::buildTailTrie() {
    Vector<Vector<char> > origTails = vtails_;
    try {
        vtailTrie_ = new Trie;
    } catch (std::bad_alloc&) {
        isReady_ = false;
        return;
    }
    for (size_t i = 0; i < vtails_.size(); ++i) {
        reverse(vtails_[i].begin(), vtails_[i].end());
    }
    vtailTrie_->build(vtails_);
    tailIDSize_ = lg2(vtailTrie_->size());

    size_t tailSize = origTails.size();
    for (size_t i = 0; i < tailSize; ++i) {
        reverse(origTails[i].begin(), origTails[i].end());
        id_t id = vtailTrie_->exactMatchSearch(&origTails[i][0], origTails[i].size());
        tailIDs_.push_back_bits(id, tailIDSize_);
    }
}

Trie::id_t Trie::exactMatchSearch(const char* str, size_t len) const {
    uint64_t nodePos = 0;
    uint64_t zeros = 0;
    size_t keyPos = 0;
    Trie::id_t id;

    while (keyPos <= len) {
        id = traverse(str, len, nodePos, zeros, keyPos);
        if (id == CAN_NOT_TRAVERSE) {
            break;
        }
        if (keyPos == (len + 1)) {
            return id;
        }
    }

    return NOT_FOUND;
}

void Trie::commonPrefixSearch(const char* str, size_t len, std::vector<id_t>& retIDs, uint64_t limit) const {
    retIDs.clear();
    uint64_t nodePos = 0;
    uint64_t zeros = 0;
    size_t keyPos = 0;
    Trie::id_t id;
    while ((id = traverse(str, len, nodePos, zeros, keyPos)) != CAN_NOT_TRAVERSE) {
        if (id != NOT_FOUND) {
            retIDs.push_back(id);
            if (retIDs.size() == limit) {
                break;
            }
        }
    }
}

void Trie::commonPrefixSearch(const char* str, size_t len, std::vector<Result>& results, uint64_t limit) const {
    results.clear();
    uint64_t nodePos = 0;
    uint64_t zeros = 0;
    size_t keyPos = 0;
    Trie::id_t id;
    while ((id = traverse(str, len, nodePos, zeros, keyPos)) != CAN_NOT_TRAVERSE) {
        if (id != NOT_FOUND) {
            results.push_back(Result(id, keyPos - 1));
            if (results.size() == limit) {
                break;
            }
        }
    }
}

void Trie::predictiveSearch(const char* str, size_t len, std::vector<id_t>& retIDs, uint64_t limit) const {
    retIDs.clear();
    if (!isReady_)
        return;
    if (limit == 0)
        return;

    uint64_t pos = 2;
    uint64_t zeros = 2;
    for (size_t i = 0; i < len; ++i) {
        uint64_t ones = pos - zeros;

        if (tail_[ones]) {
            uint64_t tailID = tail_.rank1(ones);
            Vector<char> tail = getTail(tailID);
            for (size_t j = i; j < len; ++j) {
                if (str[j] != tail[j - i]) {
                    return;
                }
            }
            retIDs.push_back(terminal_.rank1(ones));
            return;
        }
        getChild((uint8_t) str[i], pos, zeros);
        if (pos == NOT_FOUND) {
            return;
        }
    }
    enumerateAll(pos, zeros, retIDs, limit);
}

bool Trie::isLeaf(const uint64_t pos) const {
    return louds_[pos];
}

void Trie::getChild(const uint8_t c, uint64_t& pos, uint64_t& zeros) const {
    for (;; ++pos, ++zeros) {
        if (louds_[pos]) {
            pos = NOT_FOUND;
            return;
        }
        if (edges_[zeros - 2] == c) {
            pos = louds_.select1(zeros - 1) + 1;
            zeros = pos - zeros + 1;
            return;
        }
    }
}

void Trie::getParent(uint8_t& c, uint64_t& pos, uint64_t& zeros) const {
    zeros = pos - zeros + 1;
    pos = louds_.select0(zeros - 1);
    if (zeros < 2)
        return;
    c = edges_[zeros - 2];
}

void Trie::enumerateAll(const uint64_t pos, const uint64_t zeros, vector<id_t>& retIDs, const size_t limit) const {
    const uint64_t ones = pos - zeros;
    if (terminal_[ones]) {
        retIDs.push_back(terminal_.rank1(ones));
    }

    for (uint64_t i = 0; louds_[pos + i] == 0 && retIDs.size() < limit; ++i) {
        uint64_t nextPos = louds_.select1(zeros + i - 1) + 1;
        enumerateAll(nextPos, nextPos - zeros - i + 1, retIDs, limit);
    }
}

bool Trie::tailMatch(const char* str, const size_t len, const size_t depth, const uint64_t tailID,
        size_t& retLen) const {
    Vector<char> tail = getTail(tailID);
    if (tail.size() > (len - depth)) {
        return false;
    }
    for (size_t i = 0; i < tail.size(); ++i) {
        if (str[i + depth] != tail[i]) {
            return false;
        }
    }
    retLen = tail.size();
    return true;
}

Vector<char> Trie::getTail(const uint64_t i) const {
    if (vtailTrie_) {
        string tail;
        vtailTrie_->decodeKey(tailIDs_.get_bits(tailIDSize_ * i, tailIDSize_), tail);
        reverse(tail.begin(), tail.end());
        Vector<char> ret;
        ret.resize(tail.size());
        std::memcpy(&ret[0], tail.c_str(), tail.size());
        return ret;
    } else {
        return vtails_[i];
    }
}

Trie::id_t Trie::traverse(const char* str, size_t len, uint64_t& nodePos, uint64_t& zeros, size_t& keyPos) const {
    id_t id = NOT_FOUND;
    if (!isReady_)
        return CAN_NOT_TRAVERSE;
    if (nodePos == NOT_FOUND)
        return CAN_NOT_TRAVERSE;
    if (keyPos > len)
        return CAN_NOT_TRAVERSE;

    uint64_t defaultPos = 2;
    nodePos = std::max(nodePos, defaultPos);
    zeros = std::max(zeros, defaultPos);
    uint64_t ones = nodePos - zeros;

    if (tail_[ones]) {
        size_t retLen = 0;
        if (tailMatch(str, len, keyPos, tail_.rank1(ones), retLen)) {
            keyPos += retLen;
            id = terminal_.rank1(ones);
        }
    } else if (terminal_[ones]) {
        id = terminal_.rank1(ones);
    }
    getChild((uint8_t) str[keyPos], nodePos, zeros);
    ++keyPos;
    if (id == NOT_FOUND && nodePos == NOT_FOUND) {
        return CAN_NOT_TRAVERSE;
    }

    return id;
}

void Trie::decodeKey(id_t id, std::string& ret) const {
    ret.clear();
    if (!isReady_)
        return;

    uint64_t nodeID = terminal_.select1(id);
    uint64_t pos = louds_.select1(nodeID) + 1;
    uint64_t zeros = pos - nodeID;
    for (;;) {
        uint8_t c = 0;
        getParent(c, pos, zeros);
        if (pos == 0)
            break;
        ret += (char) c;
    }
    reverse(ret.begin(), ret.end());
    if (tail_[nodeID]) {
        Vector<char> tail = getTail(tail_.rank1(nodeID));
        ret.append(&tail[0], tail.size());
    }
}

void Trie::swap(Trie& x) {
    louds_.swap(x.louds_);
    terminal_.swap(x.terminal_);
    tail_.swap(x.tail_);
    vtails_.swap(x.vtails_);
    edges_.swap(x.edges_);
    tailIDs_.swap(x.tailIDs_);
    std::swap(numOfKeys_, x.numOfKeys_);
    std::swap(isReady_, x.isReady_);
    std::swap(vtailTrie_, x.vtailTrie_);
    std::swap(tailIDSize_, x.tailIDSize_);
}

void Trie::clear() {
    Trie tmpTrie;
    swap(tmpTrie);
}

size_t Trie::size() const {
    return numOfKeys_;
}

void Trie::save(std::ostream& os) const throw (hsds::Exception) {
    louds_.save(os);
    terminal_.save(os);
    tail_.save(os);

    os.write(reinterpret_cast<const char*>(&numOfKeys_), sizeof(numOfKeys_));
    edges_.save(os);

    bool useTailTrie = (vtailTrie_ != NULL);
    os.write((const char*) &useTailTrie, sizeof(useTailTrie));
    if (useTailTrie) {
        vtailTrie_->save(os);
        tailIDs_.save(os);
    } else {
        size_t vtailSize = vtails_.size();
        os.write(reinterpret_cast<const char*>(&vtailSize), sizeof(vtailSize));
        for (size_t i = 0; i < vtailSize; ++i) {
            vtails_[i].save(os);
        }
    }
}

void Trie::load(std::istream& is) throw (hsds::Exception) {
    clear();
    louds_.load(is);
    terminal_.load(is);
    tail_.load(is);
    is.read(reinterpret_cast<char*>(&numOfKeys_), sizeof(numOfKeys_));
    size_t edgesSize = 0;
    is.read(reinterpret_cast<char*>(&edgesSize), sizeof(edgesSize));
    edges_.resize(edgesSize);
    is.read(reinterpret_cast<char*>(&edges_[0]), sizeof(edges_[0]) * edgesSize);

    bool useTailTrie = false;
    is.read(reinterpret_cast<char*>(&useTailTrie), sizeof(useTailTrie));
    if (useTailTrie) {
        vtailTrie_ = new Trie();
        vtailTrie_->load(is);
        tailIDs_.load(is);
        tailIDSize_ = lg2(vtailTrie_->size());
    } else {
        size_t vtailSize = 0;
        is.read(reinterpret_cast<char*>(&vtailSize), sizeof(vtailSize));
        vtails_.resize(vtailSize);
        for (size_t i = 0; i < vtailSize; ++i) {
            vtails_[i].load(is);
        }
    }

    HSDS_EXCEPTION_IF(is.fail(), E_LOAD_FILE);
    isReady_ = true;
}

uint64_t Trie::map(void *ptr, uint64_t mapSize) throw (hsds::Exception) {
    clear();
    uint64_t offset = 0;
    offset += louds_.map(ptr, mapSize);
    offset += terminal_.map(reinterpret_cast<char*>(ptr) + offset, mapSize - offset);
    offset += tail_.map(reinterpret_cast<char*>(ptr) + offset, mapSize - offset);

    numOfKeys_ = *(reinterpret_cast<char*>(ptr) + offset);
    offset += sizeof(numOfKeys_);
    offset += edges_.map(reinterpret_cast<char*>(ptr) + offset, mapSize - offset);

    bool useTailTrie = false;
    useTailTrie = *(reinterpret_cast<char*>(ptr) + offset);
    offset += sizeof(bool);

    if (useTailTrie) {
        vtailTrie_ = new Trie();
        offset += vtailTrie_->map(reinterpret_cast<char*>(ptr) + offset, mapSize - offset);
        offset += tailIDs_.map(reinterpret_cast<char*>(ptr) + offset, mapSize - offset);
        tailIDSize_ = lg2(vtailTrie_->size());
    } else {
        size_t vtailSize = *(reinterpret_cast<char*>(ptr) + offset);
        offset += sizeof(vtailSize);
        vtails_.resize(vtailSize);
        for (size_t i = 0; i < vtailSize; ++i) {
            offset += vtails_[i].map(reinterpret_cast<char*>(ptr) + offset, mapSize - offset);
        }
    }

    isReady_ = true;

    return offset;
}

}
