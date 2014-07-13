
#include "hsds/trie.hpp"
#include <algorithm>
#include <queue>

namespace hsds {

using namespace std;

struct RangeNode{
  RangeNode(size_t _left, size_t _right) :
    left(_left), right(_right) {}
  size_t left;
  size_t right;
};

Trie::Trie() : louds_(), 
               terminal_(),
               tail_(),
               vtails_(),
               edges_(),
               numOfKeys_(0),
               isReady_(false){

}

Trie::~Trie() {}


void Trie::build(vector<string>& keyList) {
    clear();
    sort(keyList.begin(), keyList.end());
    keyList.erase(unique(keyList.begin(), keyList.end()), keyList.end());

    numOfKeys_ = keyList.size();

    queue<RangeNode> q;
    queue<RangeNode> nextQ;

    if (numOfKeys_ != 0){
        q.push(RangeNode(0, numOfKeys_));
    }

    BitVector loudBV, terminalBV, tailBV;
    size_t loud_index = 0;
    size_t terminal_index = 0;
    size_t tail_index = 0;
    loudBV.set(loud_index++, false);
    loudBV.set(loud_index++, true);

    for(size_t depth = 0;;){
        if(q.empty()){
            std::swap(q, nextQ);
            ++depth;
            if(q.empty()){
                break;
            }
        }
        RangeNode& rn = q.front();
        const size_t left  = rn.left;
        const size_t right = rn.right;
        q.pop();
        string& cur = keyList[left];
        if (left + 1 == right &&
            depth + 1 < cur.size()){ // tail candidate
            loudBV.set(loud_index++, true);
            terminalBV.set(terminal_index++, true);
            tailBV.set(tail_index++, true);
            string tail;
            for (size_t i = depth; i < cur.size(); ++i){
                tail += cur[i];
            }
            vtails_.push_back(tail);
            continue;
        } else {
            tailBV.set(tail_index++, false);
        }
    
        size_t newLeft = left;
        if (depth == cur.size()){
            terminalBV.set(terminal_index++, true);
            ++newLeft;
            if (newLeft == right){
                loudBV.set(loud_index++, true);
                continue;
            }
        } else {
            terminalBV.set(terminal_index++, false);
        }
    
        size_t  prev  = newLeft;
        uint8_t prevC = (uint8_t)keyList[prev][depth];
        uint64_t degree = 0;
        for (size_t i = prev+1; ; ++i){
            if (i < right && 
                prevC == (uint8_t)keyList[i][depth]){
                continue;
            }
            edges_.push_back(prevC);
            loudBV.set(loud_index++, false);
            degree++;
            nextQ.push(RangeNode(prev, i));
            if (i == right){
                break;
            }
            prev  = i;
            prevC = keyList[prev][depth];
      
        }
        loudBV.set(loud_index++, true);
    }

    loudBV.build(true, true);
    louds_.swap(loudBV);
    terminalBV.build(true);
    terminal_.swap(terminalBV);
    tailBV.build();
    tail_.swap(tailBV);

    if(numOfKeys_ > 0){
        isReady_ = true;
    }
}

Trie::id_t Trie::exactMatchSearch(const char* str, size_t len) const {
    uint64_t nodePos = 0;
    uint64_t zeros = 0;
    size_t keyPos = 0;
    Trie::id_t id;

    while(keyPos <= len){
        id = traverse(str, len, nodePos, zeros, keyPos);
        if(keyPos == (len + 1)  && id != CAN_NOT_TRAVERSE){
            return id;
        }
    }

    return NOT_FOUND;
}
 
void Trie::commonPrefixSearch(const char* str,
                              size_t len,
                              std::vector<id_t>& retIDs,
                              uint64_t limit) const {
    retIDs.clear();
    uint64_t nodePos = 0;
    uint64_t zeros = 0;
    size_t keyPos = 0;
    Trie::id_t id;
    while((id = traverse(str, len, nodePos, zeros, keyPos)) != CAN_NOT_TRAVERSE){
        if(id != NOT_FOUND){
            retIDs.push_back(id);
            if(retIDs.size() == limit){
                break;
            }
        }
    }
}

void Trie::commonPrefixSearch(const char* str,
                              size_t len,
                              std::vector<Result>& results,
                              uint64_t limit) const {
    results.clear();
    uint64_t nodePos = 0;
    uint64_t zeros = 0;
    size_t keyPos = 0;
    Trie::id_t id;
    while((id = traverse(str, len, nodePos, zeros, keyPos)) != CAN_NOT_TRAVERSE){
        if(id != NOT_FOUND){
            results.push_back(Result(id, keyPos - 1));
            if(results.size() == limit){
                break;
            }
        }
    }
}
 
void Trie::predictiveSearch(const char* str,
                           size_t len,
                           std::vector<id_t>& retIDs,
                           uint64_t limit) const {
    retIDs.clear();
    if (!isReady_) return;
    if (limit == 0) return;

    uint64_t pos       = 2;
    uint64_t zeros     = 2;
    for (size_t i = 0; i < len; ++i){
        uint64_t ones = pos - zeros;

        if (tail_[ones]){
            uint64_t tailID = tail_.rank1(ones);
            string tail = getTail(tailID);
            for (size_t j = i; j < len; ++j){
                if (str[j] != tail[j-i]){
                    return;
                }
            }
            retIDs.push_back(terminal_.rank1(ones));
            return;
        }
        getChild((uint8_t)str[i], pos, zeros);
        if (pos == NOT_FOUND){
            return;
        }
    }
    enumerateAll(pos, zeros, retIDs, limit);
}

bool Trie::isLeaf(const uint64_t pos) const {
    return louds_[pos];
}

void Trie::getChild(const uint8_t c, uint64_t& pos, uint64_t& zeros) const {
    for (;; ++pos, ++zeros){
        if (louds_[pos]){
            pos = NOT_FOUND;
            return;
        }
        if (edges_[zeros-2] == c){
            pos   = louds_.select1(zeros - 1) + 1;
            zeros = pos - zeros + 1;
            return;
        }
    }
}

void Trie::getParent(uint8_t& c, uint64_t& pos, uint64_t& zeros) const {
    zeros = pos - zeros + 1;
    pos   = louds_.select0(zeros - 1);
    if (zeros < 2) return;
    c     = edges_[zeros-2];
}

void Trie::enumerateAll(const uint64_t pos, const uint64_t zeros, vector<id_t>& retIDs, const size_t limit) const{
    const uint64_t ones = pos - zeros;
    if (terminal_[ones]){
        retIDs.push_back(terminal_.rank1(ones));
    }

    for (uint64_t i = 0; louds_[pos + i] == 0 &&
            retIDs.size() < limit; ++i){
        uint64_t nextPos = louds_.select1(zeros + i - 1)+1;
        enumerateAll(nextPos, nextPos - zeros - i + 1,  retIDs, limit);
    }
}

bool Trie::tailMatch(const char* str, const size_t len, const size_t depth,
        const uint64_t tailID, size_t& retLen) const{
    string tail = getTail(tailID);
    if (tail.size() > (len - depth)) {
        return false;
    }
    for (size_t i = 0; i < tail.size(); ++i){
        if (str[i+depth] != tail[i]) {
            return false;
        }
    }
    retLen = tail.size();
    return true;
}

std::string Trie::getTail(const uint64_t i) const{
    return vtails_[i];
}

Trie::id_t Trie::traverse(const char* str, size_t len, uint64_t& nodePos, uint64_t& zeros, size_t& keyPos) const {
    id_t id = NOT_FOUND;
    if(!isReady_) return CAN_NOT_TRAVERSE;
    if(nodePos == NOT_FOUND) return CAN_NOT_TRAVERSE;
 
    uint64_t defaultPos = 2;
    nodePos = std::max(nodePos, defaultPos);
    zeros = std::max(zeros, defaultPos);
    uint64_t ones = nodePos - zeros;

    if (tail_[ones]){
        size_t retLen = 0;
        if (tailMatch(str, len, keyPos, tail_.rank1(ones), retLen)){
            keyPos += retLen;
            id = terminal_.rank1(ones);
        }
    } else if (terminal_[ones]){
        id = terminal_.rank1(ones);
    }

    getChild((uint8_t)str[keyPos], nodePos, zeros);
    
    ++keyPos;
    if(id == NOT_FOUND && nodePos == NOT_FOUND){
        return CAN_NOT_TRAVERSE;
    }

    return id;
}
 
void Trie::decodeKey(id_t id, std::string& ret) const {
    ret.clear();
    if (!isReady_) return;

    uint64_t nodeID = terminal_.select1(id);

    uint64_t pos    = louds_.select1(nodeID) + 1;
    uint64_t zeros  = pos - nodeID;
    for (;;) { 
        uint8_t c = 0;
        getParent(c, pos, zeros);
        if (pos == 0) break;
        ret += (char)c;
    }
    reverse(ret.begin(), ret.end());
    if (tail_[nodeID]){
        ret += getTail(tail_.rank1(nodeID));
    }
}

void Trie::swap(Trie& x) {
    louds_.swap(x.louds_);
    terminal_.swap(x.terminal_);
    tail_.swap(x.tail_);
    vtails_.swap(x.vtails_);
    edges_.swap(x.edges_);
    std::swap(numOfKeys_, x.numOfKeys_);
    std::swap(isReady_, x.isReady_);
}

void Trie::clear() {
    Trie tmpTrie;
    swap(tmpTrie);
}


}
