#include <igloo/igloo_alt.h>
#include <string>
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
        AssertThatEx(wm->size(), Is().EqualTo(0UL));
        delete wm;
    }

    It(T002_build_and_access) {
    // https://code.google.com/p/wat-array/wiki/WatArrayIntro
        WaveletMatrix wm;
        vector<uint64_t> v;
        v.push_back(5);
        v.push_back(1);
        v.push_back(0);
        v.push_back(4);
        v.push_back(2);
        v.push_back(2);
        v.push_back(0);
        v.push_back(3);

        wm.build(v);

        for (size_t i = 0; i < v.size(); ++i) {
            AssertThatEx(wm.lookup(i), Is().EqualTo(v[i]));
            AssertThatEx(wm[i], Is().EqualTo(v[i]));
        }

        uint64_t pos = 0, val = 0;
        AssertThatEx( wm.rank(3, 6), Is().EqualTo(0UL)); // =0 ... The number of 3 in vec[0..6)
        AssertThatEx( wm.rank(0, 6), Is().EqualTo(1UL)); // =2 ... The number of 0 in vec[0..6)
        AssertThatEx( wm.rank(0, 7), Is().EqualTo(2UL)); // =2 ... The number of 2 in vec[0..7)
        AssertThatEx( wm.rank(2, 6), Is().EqualTo(2UL)); // =2 ... The number of 2 in vec[0..5)
        AssertThatEx( wm.rank(5, 6), Is().EqualTo(1UL)); // =1 ... The number of 5 in vec[0..6)
        AssertThatEx( wm.rank(1, 10), Is().EqualTo(hsds::NOT_FOUND)); // pos > wm.size()
        AssertThatEx( wm.select(2, 1), Is().EqualTo(4UL)); // =4 ... The second 2 appeared in vec[4]
        AssertThatEx( wm.select(2, 2), Is().EqualTo(5UL)); // =5 ... The second 2 appeared in vec[5] // =6 ... The second 2 appeared in vec[6]
        AssertThatEx( wm.rankLessThan(4, 5), Is().EqualTo(3UL)); // =3 .. {1,0,2}  appear in vec[0..5)
        AssertThatEx( wm.rankMoreThan(3, 5), Is().EqualTo(2UL)); // =2 ... {5, 4} appear in vec[0..5)
        AssertThatEx( wm.freq(2), Is().EqualTo(2UL)); // =2 ... {2, 2} appear in vec
        AssertThatEx( wm.freqRange(2, 5, 2, 6), Is().EqualTo(3UL)); // = 3 ... {4, 2, 2} appear in A[2...5)
        AssertThatEx( wm.freqSum(0,3), Is().EqualTo(5UL)); // =5 0 <= c < 3 = (0,0,1,2,2)

        wm.maxRange(1, 6, pos, val); // =(pos=3, val=4). A[3]=4 is the maximum in vec[1...6)
        AssertThatEx( pos, Is().EqualTo(3UL));
        AssertThatEx( val, Is().EqualTo(4UL));

        wm.minRange(1, 6, pos, val); // =(pos=2, val=0) A[6]=0 is minimum in vec[3..6)
        AssertThatEx( pos, Is().EqualTo(2UL));
        AssertThatEx( val, Is().EqualTo(0UL));

        wm.quantileRange(1, 6, 3, pos, val); // = (pos=4, val=2). Sort A[1...6) = 01224, and take the (3+1)-th value
        AssertThatEx( pos, Is().EqualTo(5UL));
        AssertThatEx( val, Is().EqualTo(2UL));


        std::vector<ListResult> result;
        wm.listModeRange(1,3, 0, 8, 3, result);
        AssertThatEx(result.size(), Is().EqualTo(2UL));
        AssertThatEx(result[0].c, Is().EqualTo(2UL));
        AssertThatEx(result[0].freq, Is().EqualTo(2UL));
        AssertThatEx(result[1].c, Is().EqualTo(1UL));
        AssertThatEx(result[1].freq, Is().EqualTo(1UL));

        result.clear();
        wm.listMaxRange(1,5, 0, 8, 3, result);
        AssertThatEx(result.size(), Is().EqualTo(3UL));
        AssertThatEx(result[0].c, Is().EqualTo(4UL));
        AssertThatEx(result[0].freq, Is().EqualTo(1UL));
        AssertThatEx(result[1].c, Is().EqualTo(3UL));
        AssertThatEx(result[1].freq, Is().EqualTo(1UL));
        AssertThatEx(result[2].c, Is().EqualTo(2UL));
        AssertThatEx(result[2].freq, Is().EqualTo(2UL));
        result.clear();
        wm.listMinRange(0,5, 0, 8, 3, result);
        AssertThatEx(result.size(), Is().EqualTo(3UL));
        AssertThatEx(result[0].c, Is().EqualTo(0UL));
        AssertThatEx(result[0].freq, Is().EqualTo(2UL));
        AssertThatEx(result[1].c, Is().EqualTo(1UL));
        AssertThatEx(result[1].freq, Is().EqualTo(1UL));
        AssertThatEx(result[2].c, Is().EqualTo(2UL));
        AssertThatEx(result[2].freq, Is().EqualTo(2UL));
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
        It(T004_save_and_mmap) {
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
                    AssertThatEx(mappedSize, Is().EqualTo((unsigned)sb.st_size));

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
