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

typedef struct rena_uint_result {
	uint64_t val;
	const char *end;
} rena_uint_result_t;

RENA_EXTERN rena_uint_result_t rena_uint_deserialize_dec(const char *start);

#if !RENA_HEADER_ONLY || RENA_IMPLEMENTATION

#if __aarch64__
#include <arm_neon.h>
#define _RENA_NEON 1
#elif __x86_64__
#include <immintrin.h>
#define _RENA_SSE2 1
#endif

#if __GNUC__
#define _RENA_ADDV_U64(x, y, out) \
	__builtin_uaddll_overflow(x, y, (long long unsigned int *)out)
#define _RENA_MULV_U64(x, y, out) \
	__builtin_umulll_overflow(x, y, (long long unsigned int *)out)
#define _RENA_UNUSED __attribute__((unused))
#else
#define _RENA_UNUSED
#error TODO: Implement overflow detecting math ops in non-gnu compliant compilers
#endif

_RENA_UNUSED RENA_EXTERN
rena_uint_result_t rena_uint_deserialize_dec(const char *const start) {
	rena_uint_result_t out = { .val = 0, .end = start };
	bool ovf = false;

	while (*out.end >= '0' && *out.end <= '9') {
		ovf |= _RENA_MULV_U64(out.val, 10, &out.val);
		ovf |= _RENA_ADDV_U64(out.val, *out.end++ - '0', &out.val);
	}

	if (ovf) out.end = NULL;
	return out;
}

#endif // !RENA_HEADER_ONLY || RENA_IMPLEMENTATION
#endif

