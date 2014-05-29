#include <igloo/igloo_alt.h>
#include <igloo/TapTestListener.h>
#include "hsds/wavelet-matrix.hpp"
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <cstdio>

#if !defined(_MSC_VER)
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

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

        for (size_t i = 0; i < v.size(); ++i) {
            AssertThatEx(wm.lookup(i), Is().EqualTo(v[i]));
        }
    }

    Describe(IO) {
        std::string tempFile;
        static vector<uint64_t> v;

        static void SetUpContext() {
            v.push_back(5);
            v.push_back(4);
            v.push_back(3);
            v.push_back(0);
            v.push_back(0);
        }

        void TearDown() {
            remove(tempFile.c_str());
        }

        It(T003_save_and_load) {
            tempFile = "t/T003.bin";
            {
                WaveletMatrix wm;
                wm.build(v);
                ofstream ofs(tempFile.c_str());
                wm.save(ofs);
            }
            {
                WaveletMatrix wm;
                ifstream ifs(tempFile.c_str());
                wm.load(ifs);

                for (size_t i = 0; i < v.size(); ++i) {
                    AssertThatEx(wm.lookup(i), Is().EqualTo(v[i]));
                }
            }
        }
#if !defined(_MSC_VER)
        It_Skip(T004_save_and_mmap) {
            tempFile = "t/T004.bin";
            {
                WaveletMatrix wm;
                wm.build(v);
                ofstream ofs(tempFile.c_str());
                wm.save(ofs);
                ofs.close();
            }
            {
                int fd = open(tempFile.c_str(), O_RDONLY, 0);
                AssertThatEx(fd != -1, Is().EqualTo(true));
                struct stat sb;
                if (fstat(fd, &sb) == -1) {
                    Assert::That(false);
                }

                void* mmapPtr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

                WaveletMatrix wm;
                uint64_t mappedSize = 0;
                try {
                    mappedSize = wm.map(mmapPtr, sb.st_size);
                    AssertThatEx(mappedSize, Is().EqualTo(sb.st_size));

                    for (size_t i = 0; i < v.size(); ++i) {
                        AssertThatEx(wm.lookup(i), Is().EqualTo(v[i]));
                    }
                } catch (hsds::Exception& e) {
                    cout << e << endl;
                    throw;
                }

            }
        }
#endif
    };

};

vector<uint64_t> wavelet_matrix::IO::v;

int main() {
    DefaultTestResultsOutput output;
    TestRunner runner(output);

    TapTestListener listener;
    runner.AddListener(&listener);

    return runner.Run();
}
