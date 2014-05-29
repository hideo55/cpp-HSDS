#include <igloo/igloo_alt.h>
#include <igloo/TapTestListener.h>
#include "hsds/wavelet-matrix.hpp"
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstdio>

using namespace std;
using namespace igloo;
using namespace hsds;

#define AssertThatEx(X,Y) Assert::That(X, Y, __FILE__, __LINE__)

Describe(wavelet_matrix) {
    It(T001_create_instance) {
        WaveletMatrix* wm = new WaveletMatrix();
        AssertThatEx(wm != NULL, Is().EqualTo(true));
        AssertThatEx(wm->size(), Is().EqualTo(0));
        delete wm;
    }

    It(T002_build_and_lookup) {
        WaveletMatrix wm;
        vector<uint64_t> v;
        v.push_back(5);
        v.push_back(4);
        v.push_back(3);
        v.push_back(0);
        v.push_back(0);
        wm.build(v);

        for(size_t i = 0; i < v.size(); ++i){
            AssertThatEx(wm.lookup(i), Is().EqualTo(v[i]));
        }
    }
};

int main() {
    DefaultTestResultsOutput output;
    TestRunner runner(output);

    TapTestListener listener;
    runner.AddListener(&listener);

    runner.Run();
}
