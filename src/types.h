#ifndef	_KERN_STDINT_H_
#define _KERN_STDINT_H_

#if 0

#include <linux/types.h>

#ifndef	__BIT_TYPES_DEFINED__
#define	__BIT_TYPES_DEFINED__
#endif

#ifndef	_INT8_T_DEFINED_
#define	_INT8_T_DEFINED_
typedef	s8 int8_t;
#endif

#ifndef	_UINT8_T_DEFINED_
#define	_UINT8_T_DEFINED_
typedef	uint8_t uint8_t;
#endif

#ifndef	_INT16_T_DEFINED_
#define	_INT16_T_DEFINED_
typedef	s16 int16_t;
#endif

#ifndef	_UINT16_T_DEFINED_
#define	_UINT16_T_DEFINED_
typedef	u16 uint16_t;
#endif

#ifndef	_INT32_T_DEFINED_
#define	_INT32_T_DEFINED_
typedef	s32 int32_t;
#endif

#ifndef	_UINT32_T_DEFINED_
#define	_UINT32_T_DEFINED_
typedef	u32 uint32_t;
#endif

#ifndef	_INT64_T_DEFINED_
#define	_INT64_T_DEFINED_
typedef	s64 int64_t;
#endif

#ifndef	_UINT64_T_DEFINED_
#define	_UINT64_T_DEFINED_
typedef	uint64_t uint64_t;
#endif

typedef	s8 int_least8_t;
typedef	uint8_t uint_least8_t;
typedef	s16 int_least16_t;
typedef	u16 uint_least16_t;
typedef	s32 int_least32_t;
typedef	u32 uint_least32_t;
typedef	s64 int_least64_t;
typedef	uint64_t uint_least64_t;

typedef	s8 int_fast8_t;
typedef	uint8_t uint_fast8_t;
typedef	s16 int_fast16_t;
typedef	u16 uint_fast16_t;
typedef	s32 int_fast32_t;
typedef	u32 uint_fast32_t;
typedef	s64 int_fast64_t;
typedef	uint64_t uint_fast64_t;

#ifndef	_INTPTR_T_DEFINED_
#define	_INTPTR_T_DEFINED_
typedef	ptrdiff_t intptr_t;
#endif

typedef	size_t uintptr_t;

typedef	ptrdiff_t intmax_t;
typedef	size_t uintmax_t;

#define CHAR_BIT 8

#define	INT8_MIN (-0x7f - 1)
#define	INT16_MIN (-0x7fff - 1)
#define	INT32_MIN (-0x7fffffff - 1)
#define	INT64_MIN (-0x7fffffffffffffffLL - 1)

#define	INT8_MAX 0x7f
#define	INT16_MAX 0x7fff
#define	INT32_MAX 0x7fffffff
#define	INT64_MAX 0x7fffffffffffffffLL

#define	UINT8_MAX 0xff
#define	UINT16_MAX 0xffff
#define	UINT32_MAX 0xffffffffU
#define	UINT64_MAX 0xffffffffffffffffULL

#define	INT_LEAST8_MIN INT8_MIN
#define	INT_LEAST16_MIN INT16_MIN
#define	INT_LEAST32_MIN INT32_MIN
#define	INT_LEAST64_MIN INT64_MIN

#define	INT_LEAST8_MAX INT8_MAX
#define	INT_LEAST16_MAX INT16_MAX
#define	INT_LEAST32_MAX INT32_MAX
#define	INT_LEAST64_MAX INT64_MAX

#define	UINT_LEAST8_MAX UINT8_MAX
#define	UINT_LEAST16_MAX	UINT16_MAX
#define	UINT_LEAST32_MAX	UINT32_MAX
#define	UINT_LEAST64_MAX	UINT64_MAX

#define	INT_FAST8_MIN INT8_MIN
#define	INT_FAST16_MIN INT16_MIN
#define	INT_FAST32_MIN INT32_MIN
#define	INT_FAST64_MIN INT64_MIN

#define	INT_FAST8_MAX INT8_MAX
#define	INT_FAST16_MAX INT16_MAX
#define	INT_FAST32_MAX INT32_MAX
#define	INT_FAST64_MAX INT64_MAX

#define	UINT_FAST8_MAX UINT8_MAX
#define	UINT_FAST16_MAX UINT16_MAX
#define	UINT_FAST32_MAX UINT32_MAX
#define	UINT_FAST64_MAX UINT64_MAX

#ifdef __LP64__
#define	INTPTR_MIN (-0x7fffffffffffffffL - 1)
#define	INTPTR_MAX 0x7fffffffffffffffL
#define	UINTPTR_MAX 0xffffffffffffffffUL
#else
#define	INTPTR_MIN (-0x7fffffffL - 1)
#define	INTPTR_MAX 0x7fffffffL
#define	UINTPTR_MAX 0xffffffffUL
#endif

#define	INTMAX_MIN INT64_MIN
#define	INTMAX_MAX INT64_MAX
#define	UINTMAX_MAX UINT64_MAX

#define	PTRDIFF_MIN INTPTR_MIN
#define	PTRDIFF_MAX INTPTR_MAX

#define	SIG_ATOMIC_MIN INT32_MIN
#define	SIG_ATOMIC_MAX INT32_MAX

#ifndef	SIZE_MAX
#define	SIZE_MAX UINTPTR_MAX
#endif

#ifndef	WCHAR_MIN
#define	WCHAR_MIN INT32_MIN
#endif
#ifndef	WCHAR_MAX
#define	WCHAR_MAX INT32_MAX
#endif

#define	WINT_MIN INT32_MIN
#define	WINT_MAX INT32_MAX

#define	INT8_C(_c) (_c)
#define	INT16_C(_c) (_c)
#define	INT32_C(_c) (_c)
#define	INT64_C(_c) __CONCAT(_c, LL)

#define	UINT8_C(_c) (_c)
#define	UINT16_C(_c) (_c)
#define	UINT32_C(_c) __CONCAT(_c, U)
#define	UINT64_C(_c) __CONCAT(_c, ULL)

#define	INTMAX_C(_c) __CONCAT(_c, LL)
#define	UINTMAX_C(_c) __CONCAT(_c, ULL)

#else

#include <stdint.h>
#include <limits.h>

#endif

#endif