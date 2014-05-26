#include <igloo/igloo_alt.h>
#include <igloo/TapTestListener.h>
#include "hsds/bit-vector.hpp"
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

Describe(bit_vector) {

    It(create_instance) {
        BitVector* bv = new BitVector();
        AssertThatEx(bv != NULL, Is().EqualTo(true));
        AssertThatEx(bv->empty(), Is().EqualTo(true));
        delete bv;
    }

    Describe(build_bit_vector) {
        hsds::BitVector bv;

        It(build_empty_vector) {
            bv.build();
            AssertThatEx(bv.size(), Is().EqualTo(0));
        }

        It(build_normal_vector) {

            bv.set(0, true);
            bv.set(100, true);
            bv.set(101, true);
            bv.set(511, true);
            bv.set(512, true);
            bv.set(1023, true);
            bv.set(1024, true);
            bv.build();
            AssertThatEx(bv.size(true), Is().EqualTo(7));
            AssertThatEx(bv[0], Is().EqualTo(true));
            AssertThatEx(bv[1], Is().EqualTo(false));
            AssertThatEx(bv[100], Is().EqualTo(true));
            AssertThatEx(bv.rank1(1), Is().EqualTo(1));
            AssertThatEx(bv.rank1(2), Is().EqualTo(1));
            AssertThatEx(bv.rank1(100), Is().EqualTo(1));
            AssertThatEx(bv.rank1(101), Is().EqualTo(2));
            AssertThatEx(bv.rank0(1), Is().EqualTo(0));
            AssertThatEx(bv.rank0(2), Is().EqualTo(1));
            AssertThatEx(bv.rank0(100), Is().EqualTo(99));
            AssertThatEx(bv.rank0(101), Is().EqualTo(99));
            AssertThatEx(bv.size(), Is().EqualTo(1025));

            AssertThatEx(bv.select1(0), Is().EqualTo(0));
            AssertThatEx(bv.select1(1), Is().EqualTo(100));
            AssertThatEx(bv.select1(2), Is().EqualTo(101));
            AssertThatEx(bv.select1(3), Is().EqualTo(511));
            AssertThatEx(bv.select1(4), Is().EqualTo(512));
            AssertThatEx(bv.select1(5), Is().EqualTo(1023));
            AssertThatEx(bv.select1(6), Is().EqualTo(1024));

            AssertThatEx(bv.size(false), Is().EqualTo(1018));
            AssertThatEx(bv.select0(0), Is().EqualTo(1));
            AssertThatEx(bv.select0(1), Is().EqualTo(2));
            AssertThatEx(bv.select0(100), Is().EqualTo(103));

            AssertThatEx(bv.empty(), Is().EqualTo(false));
        }
    };

    It(clear_vector) {
        hsds::BitVector bv;
        for (size_t i = 0; i < 100; ++i) {
            bv.set(i, true);
        }
        bv.build();
        bv.clear();
        AssertThatEx(bv.size(), Is().EqualTo(0));
    }

    Describe(bit_vector_opration) {
        hsds::BitVector bv;
        std::string tempfile;

        void SetUp() {
            //char* tempname = mktemp("tmpXXXX");
            //tempfile = tempname;
            tempfile = "tmp001";

            bv.clear();
            bv.set(0, true);
            bv.set(100, true);
            bv.set(101, true);
            bv.set(511, true);
            bv.set(512, true);
            bv.set(1023, true);
            bv.set(1024, true);
            bv.build();
        }

        void TearDown() {
            remove(tempfile.c_str());
        }

        It(save_and_load_bit_vector) {
            ofstream ofs(tempfile.c_str(), ios_base::binary);
            bv.save(ofs);
            ofs.close();
            ifstream ifs(tempfile.c_str());

            hsds::BitVector bv2;
            bv2.load(ifs);

            AssertThatEx(bv2.size(), Is().EqualTo(bv.size()));
            AssertThatEx(bv2.size(true), Is().EqualTo(bv.size(true)));
            AssertThatEx(bv2.size(false), Is().EqualTo(bv.size(false)));
        }

        It(save_and_mmap_bit_vector) {
            ofstream ofs(tempfile.c_str(), ios_base::binary);
            bv.save(ofs);
            ofs.close();

            int fd = open(tempfile.c_str(), O_RDONLY, 0);
            AssertThatEx(fd != -1, Is().EqualTo(true));
            struct stat sb;
            if (fstat(fd, &sb) == -1) {
                Assert::That(false);
            }

            void* mmapPtr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

            {
                hsds::BitVector bv2;
                bv2.map(mmapPtr, sb.st_size);

                AssertThatEx(bv2.size(), Is().EqualTo(bv.size()));
                AssertThatEx(bv2.size(true), Is().EqualTo(bv.size(true)));
                AssertThatEx(bv2.size(false), Is().EqualTo(bv.size(false)));
            }

            munmap(mmapPtr, sb.st_size);
        }

        It(swap_bit_vector) {
            size_t origSize = bv.size();
            hsds::BitVector bv2;
            bv2.swap(bv);

            AssertThatEx(bv.size(), Is().EqualTo(0));
            AssertThatEx(bv.empty(), Is().EqualTo(true));

            AssertThatEx(bv2.size(), Is().EqualTo(origSize));
            AssertThatEx(bv2.empty(), Is().EqualTo(false));
            AssertThatEx(bv2[1024], Is().EqualTo(true));
        }

    };
};

int main() {
    DefaultTestResultsOutput output;
    TestRunner runner(output);

    TapTestListener listener;
    runner.AddListener(&listener);

    runner.Run();
}
