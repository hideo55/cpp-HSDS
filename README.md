[![Build Status](https://travis-ci.org/hideo55/cpp-HSDS.svg?branch=master)](http://travis-ci.org/hideo55/cpp-HSDS)

# HSDS - Hide's Succinct Data Structure library collection

## Install

```
$ git clone git://github.com/hideo55/cpp-HSDS.git
$ cd cpp-HSDS
$ git submodule init
$ git submodule update
$ cmake .
$ make && make install
```

## Libraries

### BitVector

`BitVector` class is implementation of Succinct Bit Vector(a.k.a. Fully Indexable Dictionary).

#### Sample

```c++
#include "hsds/bit-vector.hpp"

using namespace hsds;

int main(){
    BitVector bv;
    bv.set(0, true);
    bv.set(100, true);
    
    ...
    
    bv.build();
    
    uint64_t pos = bv.select1(0); // =0
    pos = bv.select1(1);          // =100
    pos = bv.select0(0);          // =1
 
    return 0;   
}

```

#### Build
```sh
$ g++ sample.cpp -o sample -lhsds-bitvector
```

### WaveletMatrix

```c++
#include "hsds/wavelet-matrix.hpp"

using namespace std;
using namespace hsds;

void main(){
    vector<uint64_t> vec;
    vec.push_back(1);
    vec.push_back(3);
    vec.push_back(1);
    vec.push_back(4);
    vec.push_back(2);
    vec.push_back(10);
    vec.push_back(1);

    WaveletMatix wm;
    
    wm.build(vec);
    
    cout << wm.lookup(3) << endl;          // =4 ... vec[3]
    cout << wm.rank(2, 6) << endl;         // =2 ... The number of 2 in vec[0..5]
    cout << wm.select(2,2) << endl;        // =5 ... The second 2 appeared in vec[5]
    cout << wm.rankLessThan(4, 5) << endl; // =3 ... {1,0,2}  appear in vec[0..5]
    cout << wm.rankMoreThan(4, 5) << endl; // =4 ... {5, 4} appear in vec[0..5]
    
    return 0;    
}

```

#### Build
```sh
$ g++ sample.cpp -o sample -lhsds-waveletmatrix
```

#### Sample

## Document

[API Documentation](http://hideo55.github.io/cpp-HSDS/)

## Author

Hideaki Ohno

## License

(The MIT License)

Copyright (c) 2013 Hideaki Ohno <hide.o.j55{at}gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the 'Software'), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

