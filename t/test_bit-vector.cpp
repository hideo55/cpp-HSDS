#include <igloo/igloo_alt.h>
#include "hsds/bit-vector.hpp"
#include <sstream>

using namespace std;
using namespace igloo;
using namespace hsds;

Describe(bit_vector) {

    It(create_instance) {
        BitVector* bv = new BitVector();
        Assert::That(bv != NULL);
        delete bv;
    }

    Describe(build_bit_vector) {
        hsds::BitVector bv;

        It(build_empty_vector) {

            bv.build();
            Assert::That(bv.size() == 0);
        }

        It(build_vector) {

            bv.set(0, true);
            bv.set(100, true);
            bv.set(101, true);
            bv.set(511, true);
            bv.set(512, true);
            bv.set(1023, true);
            bv.set(1024, true);
            bv.build();
            Assert::That(bv.size(true), Is().EqualTo(7));
            Assert::That(bv[0], Is().EqualTo(true));
            Assert::That(bv[1], Is().EqualTo(false));
            Assert::That(bv[100], Is().EqualTo(true));
            Assert::That(bv.rank(1), Is().EqualTo(1));
            Assert::That(bv.rank(2), Is().EqualTo(1));
            Assert::That(bv.rank(2), Is().EqualTo(1));
            Assert::That(bv.rank(100), Is().EqualTo(1));
            Assert::That(bv.rank(101), Is().EqualTo(2));
            Assert::That(bv.size(), Is().EqualTo(1025));

            Assert::That(bv.select1(0), Is().EqualTo(0));
            Assert::That(bv.select1(1), Is().EqualTo(100));
            Assert::That(bv.select1(2), Is().EqualTo(101));
            Assert::That(bv.select1(3), Is().EqualTo(511));
            Assert::That(bv.select1(4), Is().EqualTo(512));
            Assert::That(bv.select1(5), Is().EqualTo(1023));
            Assert::That(bv.select1(6), Is().EqualTo(1024));

            Assert::That(bv.size(false), Is().EqualTo(1018));
            Assert::That(bv.select0(0), Is().EqualTo(1));
            Assert::That(bv.select0(1), Is().EqualTo(2));
            Assert::That(bv.select0(100), Is().EqualTo(103));
        }
    };
};

int main(int argc, const char** argv) {
    return TestRunner::RunAllTests(argc, argv);
}
