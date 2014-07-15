#include <sys/time.h>

#include <cassert>
#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include "timer.hpp"
#include "hsds/bit-vector.hpp"
#if defined(USE_UX)
#include <ux/ux.hpp>
#endif
#if defined(USE_MARISA)
#include "marisa/grimoire/vector/bit-vector.h"
#endif

const size_t MIN_NUM_BITS = 1U << 10;
const size_t MAX_NUM_BITS = 1U << 30;
const size_t NUM_TRIALS = 11;
const size_t NUM_QUERIES = 1 << 20;

uint32_t xor128() {
    static uint32_t x = 123456789;
    static uint32_t y = 362436069;
    static uint32_t z = 521288629;
    static uint32_t w = 88675123;

    const uint32_t t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
    return w;
}

void generate_data(size_t size, double ones_ratio, std::vector<bool> *bits, std::vector<uint32_t> *point_queries,
        std::vector<uint32_t> *rank_queries, std::vector<uint32_t> *select_queries) {
    bits->resize(size);
    point_queries->resize(NUM_QUERIES);
    rank_queries->resize(NUM_QUERIES);
    select_queries->resize(NUM_QUERIES);
    const uint64_t threshold = (uint64_t) ((1ULL << 32) * ones_ratio);
    size_t num_ones = 0;
    for (size_t i = 0; i < bits->size(); ++i) {
        const bool bit = xor128() < threshold;
        (*bits)[i] = bit;
        if (bit) {
            ++num_ones;
        }
    }
    for (size_t i = 0; i < point_queries->size(); ++i) {
        (*point_queries)[i] = xor128() % bits->size();
    }
    for (size_t i = 0; i < rank_queries->size(); ++i) {
        (*rank_queries)[i] = xor128() % bits->size();
    }
    for (size_t i = 0; i < select_queries->size(); ++i) {
        (*select_queries)[i] = xor128() % num_ones;
    }
}

void benchmark_hsds(const std::vector<bool> &bits, const std::vector<uint32_t> &point_queries,
        const std::vector<uint32_t> &rank_queries, const std::vector<uint32_t> &select_queries) {

    hsds::BitVector dic;
    for (size_t i = 0; i < bits.size(); ++i) {
        dic.set(i, bits[i]);
    }
    dic.build();

    {
        std::vector<double> times;
        for (size_t i = 0; i < NUM_TRIALS; ++i) {
            Timer timer;
            uint64_t total = 0;
            for (size_t j = 0; j < point_queries.size(); ++j) {
                total += dic[point_queries[j]];
            }
            times.push_back(timer.elapsed());
            assert(total != uint64_t(-1));
        }
        std::cout << '\t' << std::setw(8) << (times[times.size() / 2] / rank_queries.size() * 1000000.0);
    }

    {
        std::vector<double> times;
        for (size_t i = 0; i < NUM_TRIALS; ++i) {
            Timer timer;
            uint64_t total = 0;
            for (size_t j = 0; j < rank_queries.size(); ++j) {
                total += dic.rank1(rank_queries[j]);
            }
            times.push_back(timer.elapsed());
            assert(total != uint64_t(-1));
        }
        std::cout << '\t' << std::setw(8) << (times[times.size() / 2] / rank_queries.size() * 1000000.0);
    }

    {
        std::vector<double> times;
        for (size_t i = 0; i < NUM_TRIALS; ++i) {
            Timer timer;
            uint64_t total = 0;

            for (size_t j = 0; j < select_queries.size(); ++j) {
                total += dic.select1(select_queries[j]);
            }
            times.push_back(timer.elapsed());
            assert(total != uint64_t(-1));
        }
        std::cout << '\t' << std::setw(8) << (times[times.size() / 2] / select_queries.size() * 1000000.0);
    }
}

void benchmark_hsds_fast(const std::vector<bool> &bits, const std::vector<uint32_t> &point_queries,
        const std::vector<uint32_t> &rank_queries, const std::vector<uint32_t> &select_queries) {

    hsds::BitVector dic;
    for (size_t i = 0; i < bits.size(); ++i) {
        dic.set(i, bits[i]);
    }
    dic.build(true); // use faster select1

    {
        std::vector<double> times;
        for (size_t i = 0; i < NUM_TRIALS; ++i) {
            Timer timer;
            uint64_t total = 0;
            for (size_t j = 0; j < point_queries.size(); ++j) {
                total += dic[point_queries[j]];
            }
            times.push_back(timer.elapsed());
            assert(total != uint64_t(-1));
        }
        std::cout << '\t' << std::setw(8) << (times[times.size() / 2] / rank_queries.size() * 1000000.0);
    }

    {
        std::vector<double> times;
        for (size_t i = 0; i < NUM_TRIALS; ++i) {
            Timer timer;
            uint64_t total = 0;
            for (size_t j = 0; j < rank_queries.size(); ++j) {
                total += dic.rank1(rank_queries[j]);
            }
            times.push_back(timer.elapsed());
            assert(total != uint64_t(-1));
        }
        std::cout << '\t' << std::setw(8) << (times[times.size() / 2] / rank_queries.size() * 1000000.0);
    }

    {
        std::vector<double> times;
        for (size_t i = 0; i < NUM_TRIALS; ++i) {
            Timer timer;
            uint64_t total = 0;

            for (size_t j = 0; j < select_queries.size(); ++j) {
                total += dic.select1(select_queries[j]);
            }
            times.push_back(timer.elapsed());
            assert(total != uint64_t(-1));
        }
        std::cout << '\t' << std::setw(8) << (times[times.size() / 2] / select_queries.size() * 1000000.0);
    }
}

#if defined(USE_UX)
void benchmark_ux(const std::vector<bool> &bits, const std::vector<uint32_t> &point_queries,
        const std::vector<uint32_t> &rank_queries, const std::vector<uint32_t> &select_queries) {
    ux::BitVec bv;
    for (size_t i = 0; i < bits.size(); ++i) {
        bv.push_back(bits[i]);
    }

    ux::RSDic dic;
    dic.build(bv);

    {
        std::vector<double> times;
        for (size_t i = 0; i < NUM_TRIALS; ++i) {
            Timer timer;
            uint64_t total = 0;
            for (size_t j = 0; j < point_queries.size(); ++j) {
                total += dic.getBit(point_queries[j]);
            }
            times.push_back(timer.elapsed());
            assert(total != uint64_t(-1));
        }
        std::cout << '\t' << std::setw(8) << (times[times.size() / 2] / rank_queries.size() * 1000000.0);
    }

    {
        std::vector<double> times;
        for (size_t i = 0; i < NUM_TRIALS; ++i) {
            Timer timer;
            uint64_t total = 0;
            for (size_t j = 0; j < rank_queries.size(); ++j) {
                total += dic.rank(rank_queries[j], 1);
            }
            times.push_back(timer.elapsed());
            assert(total != uint64_t(-1));
        }
        std::cout << '\t' << std::setw(8) << (times[times.size() / 2] / rank_queries.size() * 1000000.0);
    }

    {
        std::vector<double> times;
        for (size_t i = 0; i < NUM_TRIALS; ++i) {
            Timer timer;
            uint64_t total = 0;
            for (size_t j = 0; j < select_queries.size(); ++j) {
                total += dic.select(select_queries[j] + 1, 1);
            }
            times.push_back(timer.elapsed());
            assert(total != uint64_t(-1));
        }
        std::cout << '\t' << std::setw(8) << (times[times.size() / 2] / select_queries.size() * 1000000.0);
    }
}
#endif /* defined(USE_UX) */

#if defined(USE_MARISA)

void benchmark_marisa(const std::vector<bool> &bits, const std::vector<uint32_t> &point_queries,
        const std::vector<uint32_t> &rank_queries, const std::vector<uint32_t> &select_queries) {
    marisa::grimoire::vector::BitVector dic;
    for (size_t i = 0; i < bits.size(); ++i) {
        dic.push_back(bits[i]);
    }
    dic.build(true, true);

    {
        std::vector<double> times;
        for (size_t i = 0; i < NUM_TRIALS; ++i) {
            Timer timer;
            uint64_t total = 0;
            for (size_t j = 0; j < point_queries.size(); ++j) {
                total += dic[point_queries[j]];
            }
            times.push_back(timer.elapsed());
            assert(total != uint64_t(-1));
        }
        std::cout << '\t' << std::setw(8) << (times[times.size() / 2] / rank_queries.size() * 1000000.0);
    }

    {
        std::vector<double> times;
        for (size_t i = 0; i < NUM_TRIALS; ++i) {
            Timer timer;
            uint64_t total = 0;
            for (size_t j = 0; j < rank_queries.size(); ++j) {
                total += dic.rank1(rank_queries[j] + 1);
            }
            times.push_back(timer.elapsed());
            assert(total != uint64_t(-1));
        }
        std::cout << '\t' << std::setw(8) << (times[times.size() / 2] / rank_queries.size() * 1000000.0);
    }

    {
        std::vector<double> times;
        for (size_t i = 0; i < NUM_TRIALS; ++i) {
            Timer timer;
            uint64_t total = 0;
            for (size_t j = 0; j < select_queries.size(); ++j) {
                total += dic.select1(select_queries[j]);
            }
            times.push_back(timer.elapsed());
            assert(total != uint64_t(-1));
        }
        std::cout << '\t' << std::setw(8) << (times[times.size() / 2] / select_queries.size() * 1000000.0);
    }
}

#endif /* defined(USE_MARISA) */

int main(int argc, char *argv[]) {
    double ONES_RATIO = 0.5;
    for (int i = 1; i < argc; ++i) {
        std::stringstream s;
        s << argv[i];
        s >> ONES_RATIO;
        if ((ONES_RATIO < 0.0) || (ONES_RATIO > 1.0)) {
            std::cerr << "error: invalid ONES_RATIO: " << ONES_RATIO << std::endl;
            return -1;
        }
    }

    std::cerr << "MIN_NUM_BITS: " << MIN_NUM_BITS << std::endl;
    std::cerr << "MAX_NUM_BITS: " << MAX_NUM_BITS << std::endl;
    std::cerr << "NUM_TRIALS: " << NUM_TRIALS << std::endl;
    std::cerr << "NUM_QUERIES: " << NUM_QUERIES << std::endl;
    std::cerr << "ONES_RATIO: " << ONES_RATIO << std::endl;

    std::cout << "#bits"
            "\thsds(get)\thsds(rank)\thsds(select)"
            "\thsds_f(get)\thsds_f(rank)\thsds_f(select)"
#if defined(USE_UX)
            "\tux(get)\tux(rank)\tux(select)"
#endif
#if defined(USE_MARISA)
            "\tmarisa(get)\tmarisa(rank)\tmarisa(select)"
#endif
<<    std::endl;
    for (size_t num_bits = MIN_NUM_BITS; num_bits <= MAX_NUM_BITS; num_bits <<= 1) {
        std::vector<bool> bits;
        std::vector<uint32_t> point_queries;
        std::vector<uint32_t> rank_queries;
        std::vector<uint32_t> select_queries;
        generate_data(num_bits, ONES_RATIO, &bits, &point_queries, &rank_queries, &select_queries);

        std::cout << num_bits;
        benchmark_hsds(bits, point_queries, rank_queries, select_queries);
        benchmark_hsds_fast(bits, point_queries, rank_queries, select_queries);
#if defined(USE_UX)
        benchmark_ux(bits, point_queries, rank_queries, select_queries);
#endif
#if defined(USE_MARISA)
        benchmark_marisa(bits, point_queries, rank_queries, select_queries);
#endif
        std::cout << std::endl;
    }

    return 0;
}

