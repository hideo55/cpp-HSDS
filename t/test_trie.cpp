#include <igloo/igloo_alt.h>
#include <igloo/TapTestListener.h>
#include "hsds/trie.hpp"

#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;
using namespace igloo;
using namespace hsds;

#define AssertThatEx(X,Y) Assert::That(X, Y, __FILE__, __LINE__)


Describe(hsds_Trie) {
    It(create_instance) {
        Trie* trie = new Trie();
        AssertThatEx(trie != NULL, Is().EqualTo(true));
        delete trie;
    }

    Describe(build_trie) {
        It(build_normal_trie) {
            std::vector<string> keyList;
            keyList.push_back("bbc");
            keyList.push_back("able");
            keyList.push_back("abc");
            keyList.push_back("abcde");
            keyList.push_back("can");

            Trie trie;
            trie.build(keyList);

            vector<string>::const_iterator iter = keyList.begin();
            vector<string>::const_iterator iter_end = keyList.end();
            for(; iter != iter_end; ++iter) {
                Trie::id_t id = trie.exactMatchSearch(iter->c_str(), iter->size());
                AssertThatEx(id, IsLessThan(keyList.size()));
                string ret;
                trie.decodeKey(id, ret);
                AssertThatEx(ret.c_str(), Equals(iter->c_str()));
            }

            vector<Trie::id_t> ids;
            trie.commonPrefixSearch("abcde", 5, ids);
            AssertThatEx(ids.size(), Equals(2));

            ids.clear();
            trie.predictiveSearch("ab", 2, ids);
            AssertThatEx(ids.size(), Equals(3));
        }
    };
};

int main() {
    DefaultTestResultsOutput output;
    TestRunner runner(output);

    TapTestListener listener;
    runner.AddListener(&listener);

    return runner.Run();
}

