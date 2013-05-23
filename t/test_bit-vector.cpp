#include <iostream>
#include <cassert>
#include "hsds/bit-vector.hpp"

using namespace std;
using namespace hsds;

int main(){
    hsds::BitVector bv;

    bv.build();
    assert( bv.size() == 0);

    bv.set(0,true);
    bv.set(100,true);
    bv.set(101,true);
    bv.set(511,true);
    bv.set(512,true);
    bv.set(1023, true);
    bv.set(1024, true);
    bv.build();
    assert( bv.size(true) == 7 );
    assert( bv[0] == true );
    assert( bv[1] == false );
    assert( bv[100] == true );
    assert(bv.rank(1) == 1);
    assert(bv.rank(2) == 1);
    assert(bv.rank(2) == 1);
    assert(bv.rank(100) == 1);
    assert(bv.rank(101) == 2);
    assert(bv.size() == 1025);

    assert( bv.select1(0) == 0 );
    assert( bv.select1(1) == 100 );
    assert( bv.select1(2) == 101 );
    assert( bv.select1(3) == 511 );
    assert( bv.select1(4) == 512 );
    assert( bv.select1(5) == 1023 );
    assert( bv.select1(6) == 1024 );
    
    assert( bv.size(false) == 1018 );
    assert( bv.select0(0) == 1 );
    assert( bv.select0(1) == 2 );
    assert( bv.select0(100) == 103 );

    return 0;
}
