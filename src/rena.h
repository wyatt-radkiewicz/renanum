/**
 * @file rena.h
 * @version 0.0.0
 * @author eklipsed
 */
#ifndef _rena_h_
#define _rena_h_

#if !RENA_HEADER_ONLY
#define RENA_EXTERN static
#else
#define RENA_EXTERN
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum rena_int_flags {
	RENA_INT_DETECT_BASE		= 0,
	RENA_INT_BASE2			= 1,
	RENA_INT_BASE8			= 2,
	RENA_INT_BASE10			= 3,
	RENA_INT_BASE16			= 4,

	RENA_INT_FORBID_UNDERSCORES	= (1 << 3),
	RENA_INT_NO_WHITESPACE		= (1 << 4),
	RENA_INT_FLAGS_MAX		= (1 << 5),
} rena_int_flags_t;

typedef enum rena_float_flags {
	RENA_FLOAT_DETECT_BASE		= 0,
	RENA_FLOAT_BASE10		= 1,
	RENA_FLOAT_BASE16		= 2,

	RENA_FLOAT_FORBID_UNDERSCORES	= (1 << 2),
	RENA_FLOAT_NO_WHITESPACE	= (1 << 3),
	RENA_FLOAT_ALLOW_LONEDOT	= (1 << 4),
	RENA_FLOAT_COMMA		= (1 << 5),
	RENA_FLOAT_FLAGS_MAX		= (1 << 6),
} rena_float_flags_t;

RENA_EXTERN int rena_uint_deserialize(rena_int_flags_t flags,
					const char *str,
					uint64_t *out);

#if !RENA_HEADER_ONLY || RENA_IMPLEMENTATION

#if __aarch64__
#include <arm_neon.h>
#define _RENA_NEON 1
#else
#define _RENA_NEON 0
#endif

#if __GNUC__
#define _RENA_ADD_OVERFLOW_U64(x, y, out) __builtin_uaddll_overflow(x, y, out)
#define _RENA_MUL_OVERFLOW_U64(x, y, out) __builtin_umulll_overflow(x, y, out)
#define _RENA_UNUSED __attribute__((unused))
#else
#error TODO: Implement overflow detecting math ops in non-gnu compliant compilers
#endif

//static void _rena_do_whitespace(const char **str) {
//	while (**str == ' ' || **str == '\t') ++*str;
//}

typedef bool (_rena_uint_deserialize_fn)(const char **str,
					uint64_t *restrict result);

static bool _rena_uint_deserialize_dec(const char **str,
					uint64_t *restrict result) {
	bool overflow = false;

	*result = 0;
	while (**str >= '0' && **str <= '9') {
		overflow |= _RENA_MUL_OVERFLOW_U64(*result, 10, result)
		|| _RENA_ADD_OVERFLOW_U64(*result, *(*str)++ - '0', result);
	}

	return !overflow;
}

_RENA_UNUSED RENA_EXTERN int rena_uint_deserialize(rena_int_flags_t flags,
						const char *str,
						uint64_t *out) {
	static _rena_uint_deserialize_fn *const perm[] = {
		[RENA_INT_NO_WHITESPACE | RENA_INT_FORBID_UNDERSCORES | RENA_INT_BASE10] = _rena_uint_deserialize_dec,
	};
	const char *const start = str;
	if (perm[flags](&str, out)) {
		return str - start;
	} else {
		return 0;
	}
}

#endif // !RENA_HEADER_ONLY || RENA_IMPLEMENTATION
#endif

