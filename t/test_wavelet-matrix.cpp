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
    It(create_instance) {
        WaveletMatrix* wm = new WaveletMatrix();
        AssertThatEx(wm != NULL, Is().EqualTo(true));
        AssertThatEx(wm->size(), Is().EqualTo(0));
        delete wm;
    }
};

int main() {
    DefaultTestResultsOutput output;
    TestRunner runner(output);

    TapTestListener listener;
    runner.AddListener(&listener);

    runner.Run();
}
