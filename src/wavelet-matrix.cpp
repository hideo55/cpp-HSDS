/*
 * wavelet-matrix.cpp
 *
 *  Created on: 2014/05/25
 *      Author: Hideaki Ohno
 */

#include <vector>
#include "hsds/wavelet-matrix.hpp"

namespace hsds {

using namespace std;

template<typename T>
WaveletMatrix<T>::WaveletMatrix() :
        size_(0), alphabet_num_(0), alphabet_bit_num_(0) {
}

template<typename T>
WaveletMatrix<T>::~WaveletMatrix() {

}

template<typename T>
void WaveletMatrix<T>::clear() {

}

template<typename T>
void WaveletMatrix<T>::build(vector<T>& src) {

}

template<typename T>
uint64_t WaveletMatrix<T>::lookup(uint64_t pos) const {

}

template<typename T>
uint64_t WaveletMatrix<T>::rank(uint64_t c, uint64_t pos) const {

}

template<typename T>
uint64_t WaveletMatrix<T>::select(uint64_t c, uint64_t rank) const {
}

template<typename T>
uint64_t WaveletMatrix<T>::selectFromPos(uint64_t c, uint64_t pos, uint64_t rank) const {
}

template<typename T>
uint64_t WaveletMatrix<T>::rankLessThan(uint64_t c, uint64_t pos) const {
}

template<typename T>
uint64_t WaveletMatrix<T>::rankMoreThan(uint64_t c, uint64_t pos) const {
}

template<typename T>
void WaveletMatrix<T>::rankAll(uint64_t c, uint64_t pos, uint64_t& rank, uint64_t& freqLessThan,
        uint64_t& freqkMoreThan) const {
}

template<typename T>
uint64_t WaveletMatrix<T>::freq(uint64_t c) const {
}

template<typename T>
uint64_t WaveletMatrix<T>::freqRange(uint64_t minC, uint64_t maxC, uint64_t begPos, uint64_t endPos) const {
}

template<typename T>
void WaveletMatrix<T>::maxRange(uint64_t begPos, uint64_t endPos, uint64_t& pos, uint64_t& val) const {
}

template<typename T>
void WaveletMatrix<T>::minRange(uint64_t begPos, uint64_t endPos, uint64_t& pos, uint64_t& val) const {
}

template<typename T>
void WaveletMatrix<T>::quantileRange(uint64_t begPos, uint64_t endPos, uint64_t k, uint64_t& pos, uint64_t& val) const {
}

template<typename T>
void WaveletMatrix<T>::listModeRange(uint64_t minC, uint64_t maxC, uint64_t begPos, uint64_t endPos, uint64_t num,
        std::vector<ListResult>& res) const {
}

template<typename T>
void WaveletMatrix<T>::listMinRange(uint64_t minC, uint64_t maxC, uint64_t begPos, uint64_t endPos, uint64_t num,
        std::vector<ListResult>& res) const {
}

template<typename T>
void WaveletMatrix<T>::listMaxRange(uint64_t minC, uint64_t maxC, uint64_t begPos, uint64_t endPos, uint64_t num,
        std::vector<ListResult>& res) const {
}

template<typename T>
void WaveletMatrix<T>::save(std::istream& is) const {
}

template<typename T>
void WaveletMatrix<T>::load(std::ostream& os) {
}

} // namespace hsds
