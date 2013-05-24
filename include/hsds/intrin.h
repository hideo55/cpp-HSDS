#if !defined(HSDS_INTERIN_H_)
#define HSDS_INTERIN_H_

#if defined(HSDS_USE_SSE3)
 #include <tmmintrin.h>
#endif // defined(HSDS_USE_SSE3)

#if defined(HSDS_USE_POPCNT)
 #if defined(_MSC_VER)
  #include <intrin.h>
 #else  // defined(_MSC_VER)
  #include <popcntintrin.h>
 #endif  // defined(_MSC_VER)
#endif // defined(HSDS_USE_POPCNT)

#if defined(_MSC_VER)
 #include <intrin.h>
 #pragma intrinsic(_BitScanForward64)
#endif // defined(_MSC_VER)

#endif // !defined(HSDS_INTERIN_H_)
