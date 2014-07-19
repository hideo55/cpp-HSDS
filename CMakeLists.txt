SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

IF(NOT DEFINED  CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build")
ENDIF(NOT DEFINED CMAKE_BUILD_TYPE)

PROJECT(HSDS CXX)

SET(serial "1.0.0")

SET(soserial "1")

SET(CMAKE_CXX_FLAGS "-Wall")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
SET(CMAKE_CXX_FLAGS_DEBUG "-g -D_DEBUG")

INCLUDE(FindSSE/FindSSE)
FindSSE ()
IF(SSE3_FOUND)
    IF(SSSE3_FOUND)
            SET(CXX_DFLAGS -DHSDS_USE_SSE3 -msse3 -mssse3)
    ENDIF(SSSE3_FOUND)
ENDIF(SSE3_FOUND)

IF(SSE4_2_FOUND)
        SET(CXX_DFLAGS ${CXX_DFLAGS} -DHSDS_USE_POPCNT -msse4.2 -mpopcnt)
ENDIF(SSE4_2_FOUND)
ADD_DEFINITIONS(${CXX_DFLAGS})

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/include" "${CMAKE_CURRENT_SOURCE_DIR}/extlib/igloo" "${CMAKE_CURRENT_SOURCE_DIR}/extlib/igloo-TapTestListener")

ADD_LIBRARY(hsds-bitvector SHARED src/bit-vector.cpp)
SET_TARGET_PROPERTIES(hsds-bitvector PROPERTIES VERSION ${serial} SOVERSION ${soserial})

ADD_LIBRARY(hsds-waveletmatrix SHARED src/wavelet-matrix.cpp)
TARGET_LINK_LIBRARIES(hsds-waveletmatrix hsds-bitvector)
SET_TARGET_PROPERTIES(hsds-waveletmatrix PROPERTIES VERSION ${serial} SOVERSION ${soserial})

ADD_LIBRARY(hsds-trie SHARED src/trie.cpp)
TARGET_LINK_LIBRARIES(hsds-trie hsds-bitvector)
SET_TARGET_PROPERTIES(hsds-trie PROPERTIES VERSION ${serial} SOVERSION ${soserial})

INSTALL(TARGETS hsds-bitvector hsds-waveletmatrix DESTINATION lib)

SET(INSTALL_HEADERS include/hsds/bit-vector.hpp include/hsds/exception.hpp include/hsds/constants.hpp include/hsds/rank-index.hpp)
SET(INSTALL_HEADERS ${INSTALL_HEADERS} include/hsds/vector.hpp include/hsds/scoped_array.hpp include/hsds/scoped_ptr.hpp)
SET(INSTALL_HEADERS ${INSTALL_HEADERS} include/hsds/wavelet-matrix.hpp)

INSTALL(FILES ${INSTALL_HEADERS} DESTINATION include/hsds)

# Testing
ENABLE_TESTING()

ADD_EXECUTABLE(t/test_bit-vector t/test_bit-vector.cpp)
TARGET_LINK_LIBRARIES(t/test_bit-vector hsds-bitvector)
ADD_TEST(NAME test_bitvector COMMAND ./t/test_bit-vector)

ADD_EXECUTABLE(t/test_wavelet-matrix t/test_wavelet-matrix.cpp)
TARGET_LINK_LIBRARIES(t/test_wavelet-matrix hsds-bitvector hsds-waveletmatrix)
ADD_TEST(NAME test_waveletmatrix COMMAND ./t/test_wavelet-matrix)

ADD_EXECUTABLE(t/test_trie t/test_trie.cpp)
TARGET_LINK_LIBRARIES(t/test_trie hsds-bitvector hsds-trie)
ADD_TEST(NAME test_trie COMMAND ./t/test_trie)

# Benchmark
OPTION(WITH_BENCHMARK "Build benchmark program" OFF)

IF(WITH_BENCHMARK)
    
    OPTION(USE_UX OFF)

    OPTION(USE_MARISA OFF)

    OPTION(UX_INCLUDE_PATH "ux-tie include path" ./)
    OPTION(UX_LIBRARY_PATH "ux-trie library path" ./)
    IF(USE_UX)
        SET(BM_DEFINITIONS -DUSE_UX)
        SET(BM_LIBS ux)
        INCLUDE_DIRECTORIES(${UX_INCLUDE_PATH})
        LINK_DIRECTORIES(${UX_LIBRARY_PATH})
    ENDIF(USE_UX)

    OPTION(MARISA_INCLUDE_PATH "marisa-trie include path" ./)
    OPTION(MARISA_LIBRARY_PATH "marisa-trie library path" ./)
    IF(USE_MARISA)
        SET(BM_DEFINITIONS ${BM_DEFINITIONS} -DUSE_MARISA)
        SET(BM_LIBS marisa)
        INCLUDE_DIRECTORIES(${MARISA_INCLUDE_PATH})
        LINK_DIRECTORIES(${MARISA_LIBRARY_PATH})
    ENDIF(USE_MARISA)

    ADD_EXECUTABLE(benchmark_bit-vector benchmark/benchmark_bit-vector.cpp)
    ADD_DEFINITIONS(${BM_DEFINITIONS})

    ADD_EXECUTABLE(benchmark_trie benchmark/benchmark_trie.cpp)

    TARGET_LINK_LIBRARIES(benchmark_bit-vector hsds-bitvector ${BM_LIBS})
    TARGET_LINK_LIBRARIES(benchmark_trie hsds-trie ${BM_LIBS})

ENDIF(WITH_BENCHMARK)


