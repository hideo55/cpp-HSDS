[![Build Status](https://secure.travis-ci.org/hideo55/cpp-HSDS.png)](http://travis-ci.org/hideo55/cpp-HSDS)

# HSDS - Hide's Succinct Data Structure library collection

## Install

```
$ git clone git://github.com/hideo55/cpp-HSDS.git
$ cd cpp-HSDS
$ cmake .
$ make && make install
```

## Libraries

### BitVector - Succinct bit vector

#### Sample
```c++
#include "hsds/bit-vector.hpp"

using namespace hsds;

void main(){
    BitVector bv;
    bv.set(0, true);
    bv.set(100, true);
    
    ...
    
    bv.build();
    
    uint64_t pos = bv.select1(0)// 0;
    pos = bv.select1(1)// 100;
    pos = bv.select0(0)// 1;
    
}

```

#### Build
```sh
$ g++ sample.cpp -o sample -lhsds-bitvector
```

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

