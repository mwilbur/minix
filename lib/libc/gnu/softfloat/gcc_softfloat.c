#include "softfloat.h"

/* float */

float32 __floatsisf(int I)
{
	return int32_to_float32(I);
}

float64 __floatsidf(int I)
{
	return int32_to_float64(I);
}

float64 __floatunsidf(unsigned int I)
{
	return int64_to_float64((long long) I);
}

int __fixsfsi(float32 A)
{
	return float32_to_int32_round_to_zero(A);
}

int __fixdfsi(float64 A)
{
	return float64_to_int32_round_to_zero(A);
}

float64 __extendsfdf2(float32 A)
{
	return float32_to_float64(A);
}

float32 __addsf3(float32 A, float32 B)
{
	return float32_add(A, B);
}

float32 __subsf3(float32 A, float32 B)
{
	return float32_sub(A, B);
}

float32 __mulsf3(float32 A, float32 B)
{
	return float32_mul(A, B);
}

float32 __divsf3(float32 A, float32 B)
{
	return float32_div(A, B);
}

int __eqsf2(float32 A, float32 B)
{
	return !float32_eq(A, B);
}

int __lesf2(float32 A, float32 B)
{
	return 1 - float32_le(A, B);
}

int __gesf2(float32 A, float32 B)
{
	return float32_le(A, B) - 1;
}

int __gtsf2(float32 A, float32 B)
{
	return float32_lt(A, B);
}

int __nesf2(float32 A, float32 B)
{
	return !float32_eq(A, B);
}

int __ltsf2(float32 A, float32 B)
{
	return -float32_lt(A, B);
}

/* double */

float64 __adddf3(float64 A, float64 B)
{
	return float64_add(A, B);
}

float64 __subdf3(float64 A, float64 B)
{
	return float64_sub(A, B);
}

float64 __muldf3(float64 A, float64 B)
{
	return float64_mul(A, B);
}

float64 __divdf3(float64 A, float64 B)
{
	return float64_div(A, B);
}

int __eqdf2(float64 A, float64 B)
{
	return !float64_eq(A, B);
}

int __ledf2(float64 A, float64 B)
{
	return 1 - float64_le(A, B);
}

int __gedf2(float64 A, float64 B)
{
	return float64_le(A, B) - 1;
}

int __gtdf2(float64 A, float64 B)
{
	return float64_lt(A, B);
}

int __nedf2(float64 A, float64 B)
{
	return !float64_eq(A, B);
}

int __ltdf2(float64 A, float64 B)
{
	return -float64_lt(A, B);
}

/* long double */

#if __SIZEOF_LONG_DOUBLE__ > 8

#if __SIZEOF_LONG_DOUBLE__ == 12
#define long_double	floatx80
#else
#define long_double	float128
#endif

long_double __extenddfxf2(float64 A)
{
#if __SIZEOF_LONG_DOUBLE__ == 12
	return float64_to_floatx80(A);
#else
	return float64_to_float128(A);
#endif
}

long_double __addxf3(long_double A, long_double B)
{
#if __SIZEOF_LONG_DOUBLE__ == 12
	return floatx80_add(A, B);
#else
	return float128_add(A, B);
#endif
}

long_double __subxf3(long_double A, long_double B)
{
#if __SIZEOF_LONG_DOUBLE__ == 12
	return floatx80_sub(A, B);
#else
	return float128_sub(A, B);
#endif
}

long_double __mulxf3(long_double A, long_double B)
{
#if __SIZEOF_LONG_DOUBLE__ == 12
	return floatx80_mul(A, B);
#else
	return float128_mul(A, B);
#endif
}

long_double __divxf3(long_double A, long_double B)
{
#if __SIZEOF_LONG_DOUBLE__ == 12
	return floatx80_div(A, B);
#else
	return float128_div(A, B);
#endif
}

int __eqxf2(long_double A, long_double B)
{
#if __SIZEOF_LONG_DOUBLE__ == 12
	return !floatx80_eq(A, B);
#else
	return !float128_eq(A, B);
#endif
}

int __lexf2(long_double A, long_double B)
{
#if __SIZEOF_LONG_DOUBLE__ == 12
	return 1 - floatx80_le(A, B);
#else
	return 1 - float128_le(A, B);
#endif
}

int __gexf2(long_double A, long_double B)
{
#if __SIZEOF_LONG_DOUBLE__ == 12
	return floatx80_le(A, B) - 1;
#else
	return float128_le(A, B) - 1;
#endif
}

int __gtxf2(long_double A, long_double B)
{
#if __SIZEOF_LONG_DOUBLE__ == 12
	return floatx80_lt(A, B);
#else
	return float128_lt(A, B);
#endif
}

int __nexf2(long_double A, long_double B)
{
#if __SIZEOF_LONG_DOUBLE__ == 12
	return !floatx80_eq(A, B);
#else
	return !float128_eq(A, B);
#endif
}

int __ltxf2(long_double A, long_double B)
{
#if __SIZEOF_LONG_DOUBLE__ == 12
	return -floatx80_lt(A, B);
#else
	return -float128_lt(A, B);
#endif
}

float64 __truncxfdf2(long_double A)
{
#if __SIZEOF_LONG_DOUBLE__ == 12
	return floatx80_to_float64(A);
#else
	return float128_to_float64(A);
#endif
}

float32 __truncxfsf2(long_double A)
{
#if __SIZEOF_LONG_DOUBLE__ == 12
	return floatx80_to_float32(A);
#else
	return float128_to_float32(A);
#endif
}

#endif
