#ifndef HSDS_RANK_INDEX_H_
#define HSDS_RANK_INDEX_H_

#include <stdint.h>

namespace hsds {

class RankIndex {
public:
    RankIndex() :
            abs_(0), rel_(0) {
    }

    void set_abs(uint32_t value) {
        abs_ = value;
    }

    void set_rel1(uint64_t value) {
        rel_ = ((rel_ & ~0x7FLLU) | (value & 0x7FLLU));
    }

    void set_rel2(uint64_t value) {
        rel_ = ((rel_ & ~(0xFFLLU << 7)) | (value & 0xFFLLU) << 7);
    }

    void set_rel3(uint64_t value) {
        rel_ = ((rel_ & ~(0xFFLLU << 15)) | (value & 0xFFLLU) << 15);
    }

    void set_rel4(uint64_t value) {
        rel_ = ((rel_ & ~(0x1FFLLU << 23)) | (value & 0x1FFLLU) << 23);
    }

    void set_rel5(uint64_t value) {
        rel_ = ((rel_ & ~(0x1FFLLU << 32)) | (value & 0x1FFLLU) << 32);
    }

    void set_rel6(uint64_t value) {
        rel_ = ((rel_ & ~(0x1FFLLU << 41)) | (value & 0x1FFLLU) << 41);
    }

    void set_rel7(uint64_t value) {
        rel_ = ((rel_ & ~(0x1FFLLU << 50)) | (value & 0x1FFLLU) << 50);
    }

    uint64_t abs() const {
        return abs_;
    }

    uint64_t rel1() const {
        return rel_ & 0x7FLLU;
    }

    uint64_t rel2() const {
        return (rel_ >> 7) & 0xFFLLU;
    }

    uint64_t rel3() const {
        return (rel_ >> 15) & 0xFFLLU;
    }

    uint64_t rel4() const {
        return (rel_ >> 23) & 0x1FFLLU;
    }

    uint64_t rel5() const {
        return (rel_ >> 32) & 0x1FFLLU;
    }

    uint64_t rel6() const {
        return (rel_ >> 41) & 0x1FFLLU;
    }

    uint64_t rel7() const {
        return (rel_ >> 50) & 0x1FFLLU;
    }

private:
    uint32_t abs_;
    uint64_t rel_;
};

}

#endif /* HSDS_RANK_INDEX_H_ */
