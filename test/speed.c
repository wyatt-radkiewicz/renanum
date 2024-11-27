#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "ek.h"
#include "strings.h"
#include "../src/rena.h"

#define GB ((double)(1024 * 1024 * 1024))

static size_t string_array_bytes(const char *strings[], size_t nstr) {
	size_t bytes = 0;
	for (size_t i = 0; i < nstr; i++) bytes += strlen(strings[i]);
	return bytes;
}

typedef void (parse_fn)(const char *str);

static void test_deserialize_speed(volatile const char *const strs[],
		size_t nstrs, parse_fn *parse, const char *parse_name) {
	const size_t iters = 1000 * 1000;
	size_t total_bytes = string_array_bytes((const char **)strs,
						nstrs) * iters;
	rena_uint_result_t result;

	fprintf(stderr, "deserialize speed test (%s)\n", parse_name);

	const clock_t start = clock();
	for (size_t iter = 0; iter < iters; iter++) {
		for (size_t str = 0; str < nstrs; str++) {
			parse((const char *)strs[str]);
		}
	}
	const double time = ((double)clock() - (double)start)
		/ (double)CLOCKS_PER_SEC;

	fprintf(stderr, "number of iterations: %zu\n", iters);
	fprintf(stderr, "time (ms): %.3f\n", time * 1000.0);
	fprintf(stderr, "throughput (GB/s): %.2f\n",
		((double)total_bytes / GB) / time);
	fprintf(stderr, "throughput (millions num/s): %.2f\n",
		(double)(STRSZ_UINT * iters) / time / (1000000.0));
}

static void strtoull__parse_fn(const char *str) {
	static volatile uint64_t x;
	x = strtoull(str, NULL, 10);
}
static void uint_deserialize_dec_parse_fn(const char *str) {
	rena_uint_result_t result = rena_uint_deserialize_dec(str);
}
bool test_uint_deserialize_dec_speed(unsigned id) {
	test_deserialize_speed(_str_uint, STRSZ_UINT,
		strtoull__parse_fn, "rena_uint_deserialize_dec strtoull");
	test_deserialize_speed(_str_uint, STRSZ_UINT,
		uint_deserialize_dec_parse_fn, "rena_uint_deserialize_dec");
	return true;
}

