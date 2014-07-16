#include <vector>
#include <string>
#include <map>
#include <iostream>
#include "hsds/trie.hpp"
#include "timer.hpp"
#if defined(USE_UX)
#include <ux/ux.hpp>
#endif

using namespace std;

const size_t NUM_TRIALS = 1;
const size_t NUM_KEYS =  10000;

static const char alphanum[] = "0123456789"
        "!@#$%^&*"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

static const int alphanumSize = sizeof(alphanum) - 1;

static void genRandomString(size_t len, std::string& str) {
    srand(time(0));
    for (size_t i = 0; i < len; ++i) {
        char c = alphanum[rand() % alphanumSize];
        str.append(&c, 1);
    }
}

static std::map<std::string, bool> GEN_STRINGS;
static void getUniqueString(std::string& str) {
    do {
        genRandomString(20, str);
    } while (GEN_STRINGS.find(str) != GEN_STRINGS.end());
    GEN_STRINGS.insert(std::make_pair(str, true));
}

void benchmark_hsds(vector<string>& keyList, double& elapsed) {
    hsds::Trie trie;
    trie.build(keyList);

    vector<string>::const_iterator iter = keyList.begin();
    vector<string>::const_iterator iter_end = keyList.end();
    Timer timer;
    for(;iter != iter_end; ++iter){
        vector<hsds::Trie::id_t> ids;
        trie.commonPrefixSearch(iter->c_str(), iter->size(), ids);
    }
    elapsed += timer.elapsed();
}

#if defined(USE_UX)
void benchmark_ux(vector<string>& keyList, double& elapsed) {
    ux::Trie trie;
    trie.build(keyList);

    vector<string>::const_iterator iter = keyList.begin();
    vector<string>::const_iterator iter_end = keyList.end();
    Timer timer;
    for(;iter != iter_end; ++iter){
        vector<ux::id_t> ids;
        trie.commonPrefixSearch(iter->c_str(), iter->size(), ids);
    }
    elapsed += timer.elapsed();
}
#endif

int main() {
    double elapsed = 0;
#if defined(USE_UX)
    double elapsed_ux = 0;
#endif
    for(size_t i = 0; i < NUM_TRIALS; ++i){
        vector<string> keyList;
        GEN_STRINGS.clear();
        for(size_t j = 0; j < NUM_KEYS; ++j) {
            string key;
            getUniqueString(key);
            keyList.push_back(key);
        }
        benchmark_hsds(keyList, elapsed);
#if defined(USE_UX)
        benchmark_ux(keyList, elapsed_ux);
#endif
    }

    cout << "hsds:" << (elapsed/NUM_TRIALS) << endl;
#if defined(USE_UX)
    cout << "ux:" << (elapsed_ux/NUM_TRIALS) << endl;
#endif

    return 0;
}
