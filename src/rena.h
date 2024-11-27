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

typedef struct rena_uint {
	uint64_t val;
	const char *end;
} rena_uint_t;

RENA_EXTERN rena_uint_t rena_uint_read_dec(const char *start);
RENA_EXTERN rena_uint_t rena_uint_read_dec_ws(const char *start);
RENA_EXTERN rena_uint_t rena_uint_read_dec_ws_sp(const char *start);

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

#if _RENA_NEON
static inline int _rena_uint_dec_16(const char *src, uint64_t *out) {
	const uint8x16_t nines = vdupq_n_u8((uint8_t)'9');
	static const uint8_t nright_plus_one[16] = {
		-16, -15, -14, -13, -12, -11, -10, -9,
		-8,  -7,  -6,  -5,  -4,  -3,  -2,  -1
	};
	uint8x16_t dgts = vld1q_u8((const uint8_t *)src);
	dgts = vsubq_u8(dgts, vdupq_n_u8((uint8_t)'0'));
	const int nright = (int)vminvq_s8(
		vandq_u8(vcgtq_u8(dgts, nines),
		vld1q_u8(nright_plus_one))) + 16;
	if (!nright) {
		*out = 0;
		return 0;
	}

	// Make ones digit first
	dgts = vrev64q_u8(dgts);
	dgts = vcombine_u8(vget_high_u8(dgts), vget_low_u8(dgts));
	const uint8x16_t zeros = vdupq_n_u8(0);
	switch (nright) {
	case 1: dgts = vextq_u8(dgts, zeros, 15); break;
	case 2: dgts = vextq_u8(dgts, zeros, 14); break;
	case 3: dgts = vextq_u8(dgts, zeros, 13); break;
	case 4: dgts = vextq_u8(dgts, zeros, 12); break;
	case 5: dgts = vextq_u8(dgts, zeros, 11); break;
	case 6: dgts = vextq_u8(dgts, zeros, 10); break;
	case 7: dgts = vextq_u8(dgts, zeros, 9); break;
	case 8: dgts = vextq_u8(dgts, zeros, 8); break;
	case 9: dgts = vextq_u8(dgts, zeros, 7); break;
	case 10: dgts = vextq_u8(dgts, zeros, 6); break;
	case 11: dgts = vextq_u8(dgts, zeros, 5); break;
	case 12: dgts = vextq_u8(dgts, zeros, 4); break;
	case 13: dgts = vextq_u8(dgts, zeros, 3); break;
	case 14: dgts = vextq_u8(dgts, zeros, 2); break;
	case 15: dgts = vextq_u8(dgts, zeros, 1); break;
	}

	// Do tens / ones
	static const uint8_t tens[16] = {
		1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10, 1, 10,
	};
	dgts = vmulq_u8(dgts, vld1q_u8(tens));

	// Do hundreds / ones
	static const uint16_t hundred[8] = {
		1, 100, 1, 100, 1, 100, 1, 100,
	};
	uint16x8_t stage2 = vpaddlq_u8(dgts);
	stage2 = vmulq_u16(stage2, vld1q_u16(hundred));

	// Do tenthousands / ones
	static const uint32_t thousand[4] = {
		1, 10000, 1, 10000
	};
	uint32x4_t stage3 = vpaddlq_u16(stage2);
	stage3 = vmulq_u32(stage3, vld1q_u32(thousand));
	
	// Do hundred million / ones
	stage3 = vpaddq_u32(stage3, vdupq_n_u32(0));
	uint64_t lo = vgetq_lane_u32(stage3, 0),
		 hi = vgetq_lane_u32(stage3, 1);
	*out = hi * 100000000 + lo;

	return nright;
}
_RENA_UNUSED RENA_EXTERN rena_uint_t rena_uint_read_dec(const char *start) {
	static const uint64_t shift_amounts[16] = {
		1ull, 10ull, 100ull, 1000ull, 10000ull, 100000ull, 1000000ull,
		10000000ull, 100000000ull, 1000000000ull, 10000000000ull,
		100000000000ull, 1000000000000ull, 10000000000000ull,
		100000000000000ull, 1000000000000000ull
	};
	rena_uint_t out;
	out.end = start;

	int ndigits = _rena_uint_dec_16(out.end, &out.val);
	if (ndigits == 16) {
		uint64_t tmp;
		out.end += ndigits;
		ndigits = _rena_uint_dec_16(out.end, &tmp);
		if (_RENA_MULV_U64(out.val, shift_amounts[ndigits], &out.val)
			|| _RENA_ADDV_U64(out.val, tmp, &out.val)) goto err;
	} else if (ndigits == 0) {
		goto err;
	}

	return out;
err:
	out.end = NULL;
	return out;
}
#else
_RENA_UNUSED RENA_EXTERN rena_uint_t rena_uint_read_dec(const char *start) {
	rena_uint_t out = { .val = 0, .end = start };
	bool ovf = false;

	while (*out.end >= '0' && *out.end <= '9') {
		ovf |= _RENA_MULV_U64(out.val, 10, &out.val);
		ovf |= _RENA_ADDV_U64(out.val, *out.end++ - '0', &out.val);
	}

	if (ovf) out.end = NULL;
	return out;
}
#endif

#endif // !RENA_HEADER_ONLY || RENA_IMPLEMENTATION
#endif

