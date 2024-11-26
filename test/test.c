#include <stdio.h>

#include "ek.h"
#include "../src/rena.h"

static bool test_uint_deserialize_dec_1(unsigned id) {
	uint64_t x;
	return rena_uint_deserialize(RENA_INT_BASE10 | RENA_INT_NO_WHITESPACE | RENA_INT_FORBID_UNDERSCORES, "0", &x) && x == 0;
}
static bool test_uint_deserialize_dec_2(unsigned id) {
	uint64_t x;
	return rena_uint_deserialize(RENA_INT_BASE10 | RENA_INT_NO_WHITESPACE | RENA_INT_FORBID_UNDERSCORES, "1234", &x) && x == 1234;
}
static bool test_uint_deserialize_dec_3(unsigned id) {
	uint64_t x;
	return rena_uint_deserialize(RENA_INT_BASE10 | RENA_INT_NO_WHITESPACE | RENA_INT_FORBID_UNDERSCORES, "1234567", &x) && x == 1234567;
}
static bool test_uint_deserialize_dec_4(unsigned id) {
	uint64_t x;
	return rena_uint_deserialize(RENA_INT_BASE10 | RENA_INT_NO_WHITESPACE | RENA_INT_FORBID_UNDERSCORES, "123412341234123412341234", &x) == 0;
}
static bool test_uint_deserialize_dec_5(unsigned id) {
	uint64_t x;
	return rena_uint_deserialize(RENA_INT_BASE10 | RENA_INT_NO_WHITESPACE | RENA_INT_FORBID_UNDERSCORES, "18446744073709551615", &x) && x == UINT64_MAX;
}

bool test_uint_deserialize_speed(unsigned id);

static test_t tests[] = {
	TEST_ADD(test_uint_deserialize_dec_1)
	TEST_ADD(test_uint_deserialize_dec_2)
	TEST_ADD(test_uint_deserialize_dec_3)
	TEST_ADD(test_uint_deserialize_dec_4)
	TEST_ADD(test_uint_deserialize_dec_5)
	TEST_ADD(test_uint_deserialize_speed)
};

int main(int argc, char **argv) {
	return tests_run_foreach(NULL, tests, arrlen(tests), stdout) ? 0 : -1;
}

