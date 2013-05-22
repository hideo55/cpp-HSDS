SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(CMAKE_CXX_FLAGS_RELEASE "-Wall -O3")

SET(CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

SET(CMAKE_BUILD_TYPE Release)

PROJECT(HSDS CXX)

SET(serial "1.0.0")

SET(soserial "1")

INCLUDE(FindSSE)
FindSSE ()
IF(SSE3_FOUND)
    IF(SSSE3_FOUND)
        ADD_DEFINITIONS(-DHSDS_USE_SSE3 -mssse3)
    ENDIF(SSSE3_FOUND)
ENDIF(SSE3_FOUND)

#ADD_DEFINITIONS(-DHSDS_USE_POPCNT -mpopcnt)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")

ADD_LIBRARY(hsds-bitvector SHARED src/bit-vector.cpp)

SET_TARGET_PROPERTIES(hsds-bitvector PROPERTIES VERSION ${serial} SOVERSION ${soserial})

INSTALL(TARGETS hsds-bitvector LIBRARY DESTINATION lib)

INSTALL(FILES include/hsds/bit-vector.hpp include/hsds/popcount.hpp include/hsds/rank-index.hpp DESTINATION include/hsds)

# Testing
ENABLE_TESTING()

ADD_EXECUTABLE(test_bit-vector t/test_bit-vector.cpp)
TARGET_LINK_LIBRARIES(test_bit-vector hsds-bitvector)

ADD_TEST(NAME test_bitvector COMMAND ./test_bit-vector)

# Benchmark
ADD_EXECUTABLE(benchmark_bit-vector benchmark/benchmark.cpp)
SET_TARGET_PROPERTIES(benchmark_bit-vector PROPERTIES INCLUDE_DIRECTORIES "/home/h-ohno/dl/marisa-0.2.4/lib;${PROJECT_SOURCE_DIR}/include")
TARGET_LINK_LIBRARIES(benchmark_bit-vector hsds-bitvector ux marisa)

