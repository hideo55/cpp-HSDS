#include <iostream>
#include "bit-vector.hpp"
#include <cassert>
#include "marisa/grimoire/vector/bit-vector.h"

using namespace std;
using namespace hsds;

int main(){
    hsds::BitVector bv;
    marisa::grimoire::vector::BitVector bv2;

    bv2.push_back(false);
    bv2.push_back(true);
    bv2.build(true,true);

    bv.build();
    assert( bv.size() == 0);

    bv.set(0,true);
    bv.set(100,true);
    bv.build();
    assert( bv.size(true) == 2 );
    assert( bv.get(0) == true );
    assert( bv.get(1) == false );
    assert( bv.get(100) == true );
    assert(bv.rank(1) == 1);
    assert(bv.rank(2) == 1);
    assert(bv.rank(2) == 1);
    assert(bv.rank(100) == 1);
    assert(bv.rank(101) == 2);
    assert(bv.size() == 101);
    cout<< bv.select1(1) << endl;
    cout << bv.select0(2) << std::endl;
    return 0;
}
