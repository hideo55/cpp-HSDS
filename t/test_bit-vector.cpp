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
    bv.set(511,true);
    bv.set(512,true);
    bv.build();
    assert( bv.size(true) == 4 );
    assert( bv.get(0) == true );
    assert( bv.get(1) == false );
    assert( bv.get(100) == true );
    assert(bv.rank(1) == 1);
    assert(bv.rank(2) == 1);
    assert(bv.rank(2) == 1);
    assert(bv.rank(100) == 1);
    assert(bv.rank(101) == 2);
    assert(bv.size() == 513);
    cout << bv.select1(0) << endl;
    cout<< bv.select1(1) << endl;
    cout << bv.select1(2) << endl;
    cout << bv.select1(3) << endl;
    cout << bv.select0(0) << std::endl;
    return 0;
}
