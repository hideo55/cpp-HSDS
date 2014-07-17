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
    It(t01_create_instance) {
        Trie* trie = new Trie();
        AssertThatEx(trie != NULL, Is().EqualTo(true));
        delete trie;
    }

    It(t02_build_normal_trie) {
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
        for (; iter != iter_end; ++iter) {
            Trie::id_t id = trie.exactMatchSearch(iter->c_str(), iter->size());
            AssertThatEx(id, IsLessThan(keyList.size()));
            string ret;
            trie.decodeKey(id, ret);
            AssertThatEx(ret.c_str(), Equals(iter->c_str()));
        }

        vector<Trie::id_t> ids;
        trie.commonPrefixSearch("abcde", 5, ids);
        AssertThatEx(ids.size(), Equals(2U));

        ids.clear();
        trie.predictiveSearch("ab", 2, ids);
        AssertThatEx(ids.size(), Equals(3U));
    }

    It(t03_commomPrefixSearch) {
        std::vector<string> keyList;
        keyList.push_back("bbc");
        keyList.push_back("able");
        keyList.push_back("abc");
        keyList.push_back("abcde");
        keyList.push_back("can");

        Trie trie;
        trie.build(keyList);

        vector<Trie::id_t> ids;
        trie.commonPrefixSearch("abcde", 5, ids);
        AssertThatEx(ids.size(), Equals(2U));
        string ret;
        trie.decodeKey(ids[0], ret);
        AssertThatEx(ret, Equals(string("abc")));
        trie.decodeKey(ids[1], ret);
        AssertThatEx(ret, Equals(string("abcde")));

        vector<Trie::Result> results;
        trie.commonPrefixSearch("abcde", 5, results);
        AssertThatEx(results.size(), Equals(2U));

        trie.decodeKey(results[0].id, ret);
        AssertThatEx(ret, Equals(string("abc")));
        AssertThatEx(results[0].depth, Equals(3U));
        trie.decodeKey(results[1].id, ret);
        AssertThatEx(ret, Equals(string("abcde")));
        AssertThatEx(results[1].depth, Equals(5U));
    }

    It(t04_predictiveSearch){

    }

};

int main() {
    DefaultTestResultsOutput output;
    TestRunner runner(output);

    TapTestListener listener;
    runner.AddListener(&listener);

    return runner.Run();
}

