#ifndef HSDS_INTERIN_H_
#define HSDS_INTERIN_H_

#ifdef HSDS_USE_SSE3
 #include <tmmintrin.h>
#endif

#ifdef HSDS_USE_POPCNT
 #ifdef _MSC_VER
  #include <intrin.h>
 #else  // _MSC_VERa
  #include <popcntintrin.h>
 #endif  // _MSC_VER
#endif // HSDS_USE_POPCNT

#ifdef _MSC_VER
 #include <intrin.h>
 #pragma intrinsic(_BitScanForward64)
#endif // _MSC_VER

#endif // HSDS_INTERIN_H_
